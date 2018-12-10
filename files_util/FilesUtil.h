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

static const int BUFFER_SIZE = 1 << 18;
static const int TRIGRAM_SIZE = 6;

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
    FilesUtil(QObject* parent = nullptr):QObject(parent){};
    QVector<QString> findFilesWith(Index&, QString);
    void removeDirectory(Index&, QString);

public slots:
    void addDirectory();

signals:
    void indexingEnds(int);
    void filesIndexed(int);
    void filesToIndexCounted(int);

private:
    void resolveInterraptionRequest();
    void addDirectoryImpl(Index&, QString&);
    void getFileTrigrams(QString&, QSet<uint64_t>&);
    void addStringTrigrams(QSet<uint64_t>&, std::string&);
    bool containsString(QString &absoluteFilepath, QString& str);
private:
    Index* index_;
    QString dir_;
//    QHash<QString, QSet<uint64_t>> files_info;
//    QHash<QString, QSet<QString>> dirs_info;
     //😺 qwerty qwerty
};


#endif //FOLDER_ANALYZER_FILES_UTIL_H
