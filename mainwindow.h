#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTreeWidgetItem>
#include "files_util/FilesUtil.h"
#include <chrono>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class same_file_finder;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void findFilesWithStr(QString);

private slots:
    void inputDirectoryNameTextChanged(const QString &arg1);
    void chooseDir();
    void addDirectoryToTrackList();
    void removeFromList();
    void directoryListItemSelectionChanged();
    void addDirectory(QString);
    void find();
    void openFile(QTreeWidgetItem*);
    void displayFilesWithStr(QVector<QString>);
private:
    //Hz
    Ui::MainWindow *ui;
    Index* index;
    std::chrono::time_point<std::chrono::steady_clock> begin;
    std::chrono::time_point<std::chrono::steady_clock> end;
//    FilesUtil files_util;
};

#endif // MAINWINDOW_H
