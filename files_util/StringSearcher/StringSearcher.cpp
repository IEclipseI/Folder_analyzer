//
// Created by roman on 14.12.18.
//

#include <QSet>
#include <functional>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "StringSearcher.h"

void StringSearcher::searchString() {
    if (str.isEmpty()) {
        emit searchEnds(QVector<QString>{}, files.size());
    }
    QVector<QString> filesWithStr;
    for (auto &file : files) {
        if (containsString(file, str))
            filesWithStr.push_back(file);
    }
    emit searchEnds(filesWithStr, files.size());
}

void StringSearcher::addStringTrigrams(QSet<uint64_t> &trigrams, std::string &str) {
    for (ptrdiff_t i = 0; i <= static_cast<ptrdiff_t>(str.size()) - TRIGRAM_SIZE; ++i) {
        uint64_t hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 8);
            hash += reinterpret_cast<unsigned char &>(str[i + j]);
        }
        trigrams.insert(hash);
    }
}

bool StringSearcher::containsString(QString &absoluteFilepath, QString &str) {
    QFile file(absoluteFilepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString buffer;
//        auto boor = std::boyer_moore_horspool_searcher(
//                str.begin(), str.end(), [](const QChar &ch) { return ch.unicode(); });
        while (buffer.append(stream.read(BUFFER_SIZE)).size() >= str.size()) {
//            auto it = std::search(buffer.begin(), buffer.end(), boor);
//            if (it != buffer.end())
//                return true;
            for (int i = 0; i <= BUFFER_SIZE - str.size(); ++i) {         //BRUTE_FORCE
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