//
// Created by roman on 02.12.18.
//

#ifndef FOLDER_ANALYZER_FILES_UTIL_H
#define FOLDER_ANALYZER_FILES_UTIL_H


#include <QObject>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QVector>

class files_util: public QObject {
    Q_OBJECT
public:
    void addDirectory(QString);
    QVector<QString> findFilesWith(QString&);
    void removeDirectory(QString);

private:
    QSet<int64_t> getFileTrigrams(QString&);
    QSet<int64_t> getStringTrigrams(QString&);
    bool containsString(QString &absoluteFilepath, QString& str);

private:
    QHash<QString, QSet<int64_t>> files_info;
    const int TRIGRAM_SIZE = 3;
    const int BUFFER_SIZE = 1 << 18;
    //😺
};


#endif //FOLDER_ANALYZER_FILES_UTIL_H
