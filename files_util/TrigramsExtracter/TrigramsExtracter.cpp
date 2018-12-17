//
// Created by roman on 11.12.18.
//

#include <QtCore/QThread>
#include <QtCore/QFile>
#include "TrigramsExtracter.h"
#include <iostream>
#include <QSet>
#include <chrono>

void TrigramsExtracter::resolveInterruptionRequest() {
    if (main_thread->isInterruptionRequested())
        throw std::exception();
}

void TrigramsExtracter::extractTrigrams() {
    QVector<QPair<QString, QSet<uint64_t>>> data;
    try {
        for (auto &filepath : files) {
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
        char buf[BUFFER_SIZE + TRIGRAM_SIZE - 1];
        qint64 read = file.read(buf, BUFFER_SIZE);
        if (read >= TRIGRAM_SIZE)
            do {
                resolveInterruptionRequest();
                addStringTrigrams(trigrams, buf, read);
                if (trigrams.size() > 80000) {
                    trigrams.clear();
                    return;
                }
                for (int i = 1; i < TRIGRAM_SIZE; ++i) {
                    buf[i - 1] = buf[BUFFER_SIZE - TRIGRAM_SIZE + i];
                }
            } while ((read = file.read(buf + TRIGRAM_SIZE - 1, BUFFER_SIZE)) > 1);
    }
    file.close();
}

void TrigramsExtracter::addStringTrigrams(QSet<uint64_t> &trigrams, const char *buffer, qint64 size) {
    for (ptrdiff_t i = 0; i <= static_cast<ptrdiff_t>(size) - TRIGRAM_SIZE; ++i) {
        uint64_t hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 8);
            hash += static_cast<unsigned char>(buffer[i + j]);
        }
        trigrams.insert(hash);
    }
}
