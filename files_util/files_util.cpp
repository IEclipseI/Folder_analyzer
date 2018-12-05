//
// Created by roman on 02.12.18.
//

#include <QtCore/QDirIterator>
#include <QtCore/QTextStream>
#include "files_util.h"
#include <iostream>

void files_util::addDirectory(QString directory) {
    QDirIterator it(directory, QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filepath = it.next();
        if (!files_info.contains(filepath)) {
            auto trigrams = getFileTrigrams(filepath);
            if (!trigrams.empty())
                files_info.insert(filepath, trigrams);
        }
    }
}

QSet<int64_t> files_util::getFileTrigrams(QString &filepath) {
    QSet<int64_t> trigrams;
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString buffer;
        int64_t buffers_read = 0;
        while (buffer.append(stream.read(BUFFER_SIZE)).size() >= TRIGRAM_SIZE) {
            trigrams.unite(getStringTrigrams(buffer));
            if (trigrams.size() > (buffers_read * BUFFER_SIZE + buffer.size()) * 93 / 100)
                return QSet<int64_t>{};
            buffer = buffer.mid(buffer.size() - (TRIGRAM_SIZE - 1), (TRIGRAM_SIZE - 1));
            buffers_read++;
        }
    }
    file.close();
    return trigrams;
}

QSet<int64_t> files_util::getStringTrigrams(QString &str) {
    QSet<int64_t> trigrams;
    for (int i = 0; i <= str.size() - TRIGRAM_SIZE; ++i) {
        int hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 16);
            hash += str[i + j].unicode();
        }
        trigrams.insert(hash);
    }
    return trigrams;
}

QVector<QString> files_util::findFilesWith(QString &str) {
    QVector<QString> containsAllTrigrams;
    if (str.isEmpty())
        return containsAllTrigrams;
    QSet<int64_t> trigrams(getStringTrigrams(str));
    for (auto it = files_info.begin(); it != files_info.end(); it++) {
        bool contains = true;
        for (auto &tri : trigrams) {
            if (!it.value().contains(tri)) {
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

bool files_util::containsString(QString &absoluteFilepath, QString &str) {
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

void files_util::removeDirectory(QString dir) {
    //TODO
}

