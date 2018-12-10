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

FilesUtil::FilesUtil(Index *index, QString directory, QObject *parent) : QObject(parent), index_(index),
                                                                         dir_(directory) {
}

void FilesUtil::resolveInterraptionRequest() {
    if (QThread::currentThread()->isInterruptionRequested())
        throw std::exception();
}

void FilesUtil::addDirectory() {
    addDirectoryImpl(*index_, dir_);
    emit indexingEnds(QDialog::Accepted);
}

void FilesUtil::addDirectoryImpl(Index &index, QString &directory) {
    try {

        if (!index.dirs_info.contains(directory)) {
            index.dirs_info.insert(directory, QSet<QString>{});
            QSet<QString> &dir = *index.dirs_info.find(directory);
            QDirIterator it(directory, QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            QVector<QString> files;
            while (it.hasNext()) {
                files.push_back(it.next());
                if (it.fileInfo().isSymLink())
                    files.pop_back();
            }
            emit filesToIndexCounted(files.size());
            int count = 0;
            for (auto &filepath : files) {
                resolveInterraptionRequest();
                if (!index.files_info.contains(filepath)) {
                    index.files_info.insert(filepath, QSet<uint64_t>{});
                    dir.insert(filepath);
                    QSet<uint64_t> &s = index.files_info.find(filepath).value();
                    getFileTrigrams(filepath, s);
                    if (s.empty()) {
                        index.files_info.remove(filepath);
                    }
                }
                emit filesIndexed(++count);
            }
        }
    } catch(...) {
        //interruption was requested
    }
}

void FilesUtil::getFileTrigrams(QString &filepath, QSet<uint64_t> &trigrams) {
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        char buf[BUFFER_SIZE];
        std::string buffer;
        while (file.read(buf, BUFFER_SIZE) > 0 && buffer.append(buf).size() >= static_cast<size_t >(TRIGRAM_SIZE)) {
            addStringTrigrams(trigrams, buffer);
            buffer = buffer.substr(buffer.size() - (TRIGRAM_SIZE - 1), buffer.size());
        }
    }
    file.close();
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

QVector<QString> FilesUtil::findFilesWith(Index &index, QString str) {
    QVector<QString> containsAllTrigrams;
    if (str.isEmpty())
        return containsAllTrigrams;
    QSet<uint64_t> trigrams{};
    std::string std_str = str.toStdString();
    (addStringTrigrams(trigrams, std_str));
    for (auto it = index.files_info.begin(); it != index.files_info.end(); it++) {
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
    QVector<QString> filesWithStr;
    for (auto &file : containsAllTrigrams) {
        if (containsString(file, str))
            filesWithStr.push_back(file);
    }
    return filesWithStr;
}

bool FilesUtil::containsString(QString &absoluteFilepath, QString &str) {
    QFile file(absoluteFilepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString buffer;
        while (buffer.append(stream.read(BUFFER_SIZE)).size() >= str.size()) {
            for (int i = 0; i <= BUFFER_SIZE - str.size(); ++i) {
                int j = 0;
                for (; str[j] == buffer[i + j] && j < str.size(); j++);
                if (j == str.size())
                    return true;
            }
            buffer = buffer.mid(buffer.size() - (str.size() - 1), (str.size() - 1));
        }
    }
    file.close();
    return false;
}

void FilesUtil::removeDirectory(Index &index, QString dir) {
    int a = 0;
    if (index.dirs_info.contains(dir)) {
        for (auto &s : *index.dirs_info.find(dir)) {
            a += index.files_info.remove(s);
        }
        index.dirs_info.remove(dir);
    }
//    std::cout <<"files_removed "  << a << "\n";
//    std::cout <<"files_info_size "  << files_info.size() << "\n";
//    std::cout <<"dir_info size "  << dirs_info.size() << "\n";
}

