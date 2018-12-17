//
// Created by roman on 11.12.18.
//

#ifndef FOLDER_ANALYZER_TRIGRAMSEXTRACTER_H
#define FOLDER_ANALYZER_TRIGRAMSEXTRACTER_H


#include <QtCore/QObject>
#include "../FilesUtil.h"

//    extern const int BUFFER_SIZE;
//    extern const int TRIGRAM_SIZE;

//static const int BUFFER_SIZE = 1 << 17;
//static const int TRIGRAM_SIZE = 3;


class TrigramsExtracter: QObject {
friend class FilesUtil;
    Q_OBJECT
private:
    TrigramsExtracter(QThread* main_thread, QVector<QString> files, QObject* parent = nullptr): QObject(parent), main_thread(main_thread), files(files){};

    void getFileTrigrams(QString&, QSet<uint64_t>&);
    void addStringTrigrams(QSet<uint64_t> &, const char*, qint64 size);
//    void addStringTrigrams(std::unordered_set<uint64_t>&, std::string &);
    void resolveInterruptionRequest();
signals:
    void extractingEnds(QVector<QPair<QString, QSet<uint64_t>>>);
    void filesProcessed(int);

private slots:
    void extractTrigrams();

private:
    QThread* main_thread;
    QVector<QString> files;
};


#endif //FOLDER_ANALYZER_TRIGRAMSEXTRACTER_H
