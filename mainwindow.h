#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTreeWidgetItem>
#include "files_util/FilesUtil.h"



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

private slots:
    void inputDirectoryNameTextChanged(const QString &arg1);
    void chooseDir();
    void addDirectoryToTrackList();
    void removeFromList();
    void directoryListItemSelectionChanged();
    void addDirectory(QString);
    void find();
    void openFile(QTreeWidgetItem*);
private:
    //Hz
    Ui::MainWindow *ui;
    Index* index;
//    FilesUtil files_util;
};

#endif // MAINWINDOW_H
