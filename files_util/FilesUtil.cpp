//
// Created by roman on 02.12.18.
//

#include <QtCore/QDirIterator>
#include <QtCore/QTextStream>
#include "FilesUtil.h"
#include <iostream>
#include <chrono>
#include <unordered_set>
#include <QHash>
#include <QtWidgets/QDialog>
#include <QtCore/QThread>
#include "TrigramsExtracter/TrigramsExtracter.h"
#include <algorithm>
#include <functional>
#include <thread>
#include <cmath>
#include <files_util/StringSearcher/StringSearcher.h>


FilesUtil::FilesUtil(Index *index, QString directory, QObject *parent) : QObject(parent), index_(index),
                                                                         dir_(directory),
                                                                         filesWithStr(QVector<QString>{}) {
}

void FilesUtil::addDirectory() {
    addDirectoryImpl(*index_, dir_);
}

void FilesUtil::addDirectoryImpl(Index &index, QString &directory) {
    if (!index.dirs_info.contains(directory)) {
        index.dirs_info.insert(directory, QSet<QString>{});
        QDirIterator it(directory, QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        QVector<QPair<QString, qint64>> files;
        while (it.hasNext()) {
            files.push_back(QPair<QString, qint64>(it.next(), it.fileInfo().size()));
            if (it.fileInfo().isSymLink())
                files.pop_back();
        }
        emit filesToIndexCounted(files.size());

        std::sort(files.begin(), files.end(),
                  [](const QPair<QString, qint64> &a, const QPair<QString, qint64> &b) { return a.second > b.second; });
        files_to_index = files.size();
        int threads_count = std::max(4u, std::thread::hardware_concurrency() * 2);
        QVector<QVector<QString>> groups(threads_count);
        for (int i = 0; i < files_to_index; i++) {
            int ind = (i / files_to_index % 2) * files_to_index +
                      (1 - 2 * (i / files_to_index % 2)) * (i % threads_count - i / files_to_index % 2);
            groups[ind].push_back(files[i].first);
        }

        for (int i = 0; i < threads_count; ++i) {
            auto *trigrams_extractor_thread = new QThread();
            auto *trigrams_extractor = new TrigramsExtracter(QThread::currentThread(), groups[i]);
            trigrams_extractor->moveToThread(trigrams_extractor_thread);
            connect(trigrams_extractor_thread, SIGNAL(started()), trigrams_extractor, SLOT(extractTrigrams()));
            connect(trigrams_extractor, SIGNAL(filesProcessed(int)), this, SLOT(updateBar(int)));

            qRegisterMetaType<QVector<QPair<QString, QSet<uint64_t>>>>("QVector<QPair<QString, QSet<uint64_t>>>");

            connect(trigrams_extractor, SIGNAL(extractingEnds(QVector<QPair<QString, QSet<uint64_t>>>)),
                    trigrams_extractor_thread, SLOT(quit()));
            connect(trigrams_extractor, SIGNAL(extractingEnds(QVector<QPair<QString, QSet<uint64_t>>>)), this,
                    SLOT(updateData(QVector<QPair<QString, QSet<uint64_t>>>)));
            connect(trigrams_extractor, SIGNAL(extractingEnds(QVector<QPair<QString, QSet<uint64_t>>>)),
                    trigrams_extractor, SLOT(deleteLater()));
            connect(trigrams_extractor_thread, SIGNAL(finished()), trigrams_extractor_thread, SLOT(deleteLater()));
            trigrams_extractor_thread->start();
        }
    }
}

void FilesUtil::updateBar(int value) {
    emit filesIndexed(value);
}

void FilesUtil::addStringTrigrams(QSet<uint64_t> &trigrams, std::string &str) {
    for (ptrdiff_t i = 0; i <= static_cast<ptrdiff_t>(str.size()) - TRIGRAM_SIZE; ++i) {
        uint64_t hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 8);
            hash += reinterpret_cast<unsigned char &>(str[i + j]);
        }
        trigrams.insert(hash);
    }
}

void FilesUtil::findFilesWith() {
    QVector<QString> containsAllTrigrams;
    if (dir_.isEmpty())
        emit filesWithStrFound(QVector<QString>{});
    QSet<uint64_t> trigrams{};
    std::string std_str = dir_.toStdString();
    addStringTrigrams(trigrams, std_str);
    for (auto it = index_->files_info.begin(); it != index_->files_info.end(); it++) {
        bool contains = true;
        for (auto &tri : trigrams) {
            if ((it.value().find(tri) == it.value().end())) {
                contains = false;
                break;
            }
        }
        if (contains) {
            containsAllTrigrams.push_back(it.key());
        }
    }
    files_to_check = containsAllTrigrams.size();
    int threads_count = std::max(4u, std::thread::hardware_concurrency() * 2);
    QVector<QVector<QString>> groups(threads_count);
    std::cout <<"#" <<cur_count << " " << files_to_check << "\n";
    for (int i = 0; i < files_to_check; i++) {
        groups[i % threads_count].push_back(containsAllTrigrams[i]);
    }
    int d=0;
    for (int i = 0; i < threads_count; ++i) {
        d +=groups[i].size();
    }
    std::cout << d << "\n";
    std::cout <<"#" <<cur_count << " " << files_to_check << "\n";
    for (int i = 0; i < threads_count; ++i) {
        auto *string_searcher_thread = new QThread();
        auto *string_searcher = new StringSearcher(groups[i], dir_);
        string_searcher->moveToThread(string_searcher_thread);

        qRegisterMetaType<QVector<QString>>("QVector<QString>");

        connect(string_searcher_thread, SIGNAL(started()), string_searcher, SLOT(searchString()));
        connect(string_searcher, SIGNAL(searchEnds(QVector<QString>, int)), string_searcher_thread, SLOT(quit()));
        connect(string_searcher, SIGNAL(searchEnds(QVector<QString>, int)), string_searcher, SLOT(deleteLater()));
        connect(string_searcher_thread, SIGNAL(finished()), string_searcher_thread, SLOT(deleteLater()));
        connect(string_searcher, SIGNAL(searchEnds(QVector<QString>, int)), this, SLOT(updateFilesWithStr(QVector<QString>, int)));

        string_searcher_thread->start();
    }
}


void FilesUtil::removeDirectory(Index &index, QString dir) {
    int a = 0;
    if (index.dirs_info.contains(dir)) {
        for (auto &s : *index.dirs_info.find(dir)) {
            a += index.files_info.remove(s);
        }
        index.dirs_info.remove(dir);
    }
    std::cout << a << "\n";
}

void FilesUtil::updateData(QVector<QPair<QString, QSet<uint64_t>>> data) {
    cur_count += data.size();
    auto &dir_info = *index_->dirs_info.find(dir_);
    for (auto it = data.begin(); it != data.end(); ++it) {
        dir_info.insert(it->first);
        index_->files_info.insert(it->first, it->second);
    }
    if (cur_count == files_to_index) {
        emit indexingEnds(QDialog::Accepted);
    }
}

void FilesUtil::updateFilesWithStr(QVector<QString> files, int files_checked) {
    for (auto &file: files) {
        filesWithStr.push_back(file);
    }
    cur_count += files_checked;
    std::cout << cur_count << " " << files_to_check << "\n";
    if (cur_count == files_to_check)
        emit filesWithStrFound(filesWithStr);
}