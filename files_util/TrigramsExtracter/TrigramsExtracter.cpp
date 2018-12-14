//
// Created by roman on 11.12.18.
//

#include <QtCore/QThread>
#include <QtCore/QFile>
#include "TrigramsExtracter.h"
#include <iostream>


void TrigramsExtracter::resolveInterruptionRequest() {
    if (main_thread->isInterruptionRequested())
        throw std::exception();
}

void TrigramsExtracter::extractTrigrams(){
    QVector<QPair<QString, QSet<uint64_t>>> data;
    try {
        for (auto &filepath : files) {
            resolveInterruptionRequest();
            data.push_back(QPair<QString, QSet<uint64_t>>(filepath, QSet<uint64_t>{}));
            QSet<uint64_t> &s = data[data.size() - 1].second;
            getFileTrigrams(filepath, s);
            emit filesProcessed(1);
        }
    } catch (...) {
        //interraption requested
    }
    emit extractingEnds(data);
}

void TrigramsExtracter::getFileTrigrams(QString &filepath, QSet<uint64_t> &trigrams) {
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        char buf[BUFFER_SIZE];
        std::string buffer;
        while (file.read(buf, BUFFER_SIZE) > 0 && buffer.append(buf, BUFFER_SIZE).size() >= static_cast<size_t >(TRIGRAM_SIZE)) {
            addStringTrigrams(trigrams, buffer);
            buffer = buffer.substr(buffer.size() - (TRIGRAM_SIZE - 1), buffer.size());
        }
    }
    file.close();
}

void TrigramsExtracter::addStringTrigrams(QSet<uint64_t> &trigrams, std::string &str) {
    for (ptrdiff_t i = 0; i <= static_cast<ptrdiff_t>(str.size()) - TRIGRAM_SIZE; ++i) {
        uint64_t hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 8);
            hash += reinterpret_cast<unsigned char &>(str[i + j]);
        }
        trigrams.insert(hash);
    }
}