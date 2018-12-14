//
// Created by roman on 14.12.18.
//

#ifndef FOLDER_ANALYZER_STRINGSEARCHER_H
#define FOLDER_ANALYZER_STRINGSEARCHER_H


#include <QtCore/QObject>
#include <QtCore/QVector>
#include "../FilesUtil.h"

class StringSearcher: QObject {
friend class FilesUtil;
Q_OBJECT
private:
    StringSearcher(QObject* parent = nullptr): QObject(parent){};

    void addStringTrigrams(QSet<uint64_t>&, std::string&);
    bool containsString(QString &absoluteFilepath, QString& str);

signals:
    void searchEnds(QVector<QString>, int);

private slots:
    void searchString(QVector<QString>, QString);

private:
};


#endif //FOLDER_ANALYZER_STRINGSEARCHER_H
