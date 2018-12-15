//
// Created by roman on 02.12.18.
//

#ifndef FOLDER_ANALYZER_FILES_UTIL_H
#define FOLDER_ANALYZER_FILES_UTIL_H


#include <QObject>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QVector>
#include <unordered_set>
#include <iostream>
#include "TrigramsExtracter/TrigramsExtracter.h"


namespace {
    const int BUFFER_SIZE = 1 << 17;
    const int TRIGRAM_SIZE = 3;
}

class FilesUtil;

class Index {
    friend  class FilesUtil;
    QHash<QString, QSet<uint64_t>> files_info;
    QHash<QString, QSet<QString>> dirs_info;
};


class FilesUtil: public QObject {
    Q_OBJECT
public:
    FilesUtil(Index* index, QString directory, QObject* parent = 0);
    FilesUtil(QObject* parent = nullptr):QObject(parent), index_(nullptr), dir_(""){};
    FilesUtil(Index* index, QObject* parent = nullptr):QObject(parent), index_(index), dir_(""){};
    void removeDirectory(Index&, QString);

public slots:
    void addDirectory();
    void updateBar(int);
    void updateData(QVector<QPair<QString, QSet<uint64_t>>>);
    void updateFilesWithStr(QVector<QString>, int);
    void findFilesWith();

signals:
    void indexingEnds(int);
    void filesIndexed(int);
    void filesToIndexCounted(int);
    void filesWithStrFound(QVector<QString>);

private:
    void addDirectoryImpl(Index&, QString&);
    void addStringTrigrams(QSet<uint64_t>&, std::string&);
private:
    Index* index_;
    QString dir_;
    int files_to_index = 0;
    int cur_count = 0;
    int files_to_check = 0;
    QVector<QString> filesWithStr;
//    QHash<QString, QSet<uint64_t>> files_info;
//    QHash<QString, QSet<QString>> dirs_info;
     //😺 qwerty qwerty
};


#endif //FOLDER_ANALYZER_FILES_UTIL_H
