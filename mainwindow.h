#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "files_util/files_util.h"

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
    void addToSearch();
    void removeFromList();
    void directoryListItemSelectionChanged();
    void search();
    void find();
private:
    //Hz
    Ui::MainWindow *ui;
    files_util filesUtil;
};

#endif // MAINWINDOW_H
