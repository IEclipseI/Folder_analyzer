//
// Created by roman on 11.12.18.
//

#include <QtCore/QThread>
#include <QtCore/QFile>
#include "TrigramsExtracter.h"
#include <iostream>
#include <QSet>
#include <chrono>

static int con = 1;

void TrigramsExtracter::resolveInterruptionRequest() {
    if (main_thread->isInterruptionRequested())
        throw std::exception();
}

void TrigramsExtracter::extractTrigrams() {
    QVector<QPair<QString, TrigramsContainer>> data;
    try {
        for (auto &filepath : files) {
            data.push_back(QPair<QString, TrigramsContainer>(filepath, TrigramsContainer{}));
            TrigramsContainer &s = data[data.size() - 1].second;
            getFileTrigrams(filepath, s);
            s.makeAccessible();
            emit filesProcessed(1);
        }
    } catch (std::exception &e) {
//        std::cout << "exception " << e.what() << "\n";
        //interraption requested
    }
    emit extractingEnds(data);
}

void TrigramsExtracter::getFileTrigrams(QString &filepath, TrigramsContainer &trigrams) {
    QFile file(filepath);
//    std::cout << filepath.toStdString() << "\n";
    QSet<trigram> tr;
    if (file.open(QIODevice::ReadOnly)) {
        char buf[BUFFER_SIZE + TRIGRAM_SIZE - 1];
        qint64 read = file.read(buf, BUFFER_SIZE) - TRIGRAM_SIZE + 1;
        if (read >= 1)
            do {
                read += 2;
//                std::cout << filepath.toStdString() << " " << read << "\n";
                resolveInterruptionRequest();
                addStringTrigrams(tr, buf, read);
                if (tr.size() > 20000) {
                    tr.clear();
                    return;
                }
                for (int i = 1; i < TRIGRAM_SIZE; ++i) {
                    buf[i - 1] = buf[BUFFER_SIZE - TRIGRAM_SIZE + i];
                }
            } while ((read = file.read(buf + TRIGRAM_SIZE - 1, BUFFER_SIZE)) >= 1);
    }
    for (auto t : tr) {
        trigrams.insert(t);
    }
    file.close();
}

void TrigramsExtracter::addStringTrigrams(QSet<trigram> &trigrams, const char *buffer, qint64 size) {
    for (ptrdiff_t i = 0; i <= static_cast<ptrdiff_t>(size) - TRIGRAM_SIZE; ++i) {
        trigram hash = 0;
        for (int j = 0; j < TRIGRAM_SIZE; ++j) {
            hash = (hash << 8);
            hash += static_cast<unsigned char>(buffer[i + j]);
        }
        trigrams.insert(hash);
    }
}
