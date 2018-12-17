#include <QDir>
#include <QFileDialog>
#include <iostream>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QHash>
#include <QtCore/QThread>
#include <QMetaType>
#include <QtGui/QDesktopServices>
#include <chrono>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indexingdialog/indexingdialog.h"
#include "indexingdialog/ui_indexingdialog.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        index(new Index()),
        begin(std::chrono::time_point<std::chrono::steady_clock>()),
        end(std::chrono::time_point<std::chrono::steady_clock>()){
    ui->setupUi(this);
    ui->removeFromListButton->setEnabled(false);
    ui->directoryList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->directoryList->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->filesWithStr->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->filesWithStr->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    connect(ui->inputDirectoryName, SIGNAL(textChanged(
                                                   const QString&)), this, SLOT(inputDirectoryNameTextChanged(
                                                                                        const QString &)));
    connect(ui->chooseDirectoryButton, SIGNAL(clicked()), this, SLOT(chooseDir()));
    connect(ui->addToTrackButton, SIGNAL(clicked()), this, SLOT(addDirectoryToTrackList()));
    connect(ui->removeFromListButton, SIGNAL(clicked()), this, SLOT(removeFromList()));
    connect(ui->directoryList, SIGNAL(itemSelectionChanged()), this, SLOT(directoryListItemSelectionChanged()));
    connect(ui->filesWithStr, SIGNAL(itemDoubleClicked(QTreeWidgetItem * , int)), this,
            SLOT(openFile(QTreeWidgetItem * )));
    int size = 400;
    ui->splitter->setSizes(QList<int>{ui->splitter->height() - size, size});
    connect(ui->find, SIGNAL(clicked()), this, SLOT(find()));
}

MainWindow::~MainWindow() {
    delete index;
    delete ui;
}

void MainWindow::inputDirectoryNameTextChanged(const QString &arg1) {
    if (QDir(arg1).exists()) {
        ui->addToTrackButton->setEnabled(true);
    } else {
        ui->addToTrackButton->setEnabled(false);
    }
}

void MainWindow::chooseDir() {
    QString directory = QFileDialog::getExistingDirectory(this);
    if (!directory.isEmpty()) ui->inputDirectoryName->setText(directory);
}

void MainWindow::addDirectoryToTrackList() {
    auto dir_path = QDir(ui->inputDirectoryName->text()).absolutePath();
    for (int i = ui->directoryList->topLevelItemCount() - 1; i >= 0; i--) {
        QTreeWidgetItem *item = ui->directoryList->topLevelItem(i);
        auto item_path = QDir(item->text(1)).absolutePath();
        if (item_path == dir_path ||
            dir_path.indexOf(item_path) == 0) {
            ui->inputDirectoryName->clear();
            return;
        } else if (item_path.indexOf(dir_path) >= 0) {
            delete ui->directoryList->takeTopLevelItem(i);
        }
    }
    auto item = new QTreeWidgetItem(ui->directoryList);
    QDir d(ui->inputDirectoryName->text());
    addDirectory(d.absolutePath());
    item->setText(0, d.dirName());
    item->setText(1, d.absolutePath());
    ui->directoryList->addTopLevelItem(item);
    ui->inputDirectoryName->clear();
}

void MainWindow::removeFromList() {
    for (auto a : ui->directoryList->selectedItems()) {
        FilesUtil().removeDirectory(*index, a->text(1));
    }
    qDeleteAll(ui->directoryList->selectedItems());
}

void MainWindow::directoryListItemSelectionChanged() {
    ui->removeFromListButton->setEnabled(ui->directoryList->selectedItems().length() != 0);
}

void MainWindow::addDirectory(QString dir) {
    begin = std::chrono::steady_clock::now();

    auto *files_util_thread = new QThread();
    auto *files_util = new FilesUtil(index, dir);
    auto *indexing_dialog = new IndexingDialog(files_util_thread, this);
    files_util->moveToThread(files_util_thread);
    connect(files_util_thread, SIGNAL(started()), files_util, SLOT(addDirectory()));
    connect(files_util, SIGNAL(filesIndexed(int)), indexing_dialog, SLOT(updateBar(int)));
    connect(files_util, SIGNAL(filesToIndexCounted(int)), indexing_dialog, SLOT(setBarRange(int)));
    connect(files_util, SIGNAL(indexingEnds(int)), files_util_thread, SLOT(quit()));
    connect(files_util, SIGNAL(indexingEnds(int)), indexing_dialog, SLOT(done(int)));
    connect(files_util, SIGNAL(indexingEnds(int)), files_util, SLOT(deleteLater()));
    connect(files_util_thread, SIGNAL(finished()), files_util_thread, SLOT(deleteLater()));
    files_util_thread->start();
    if (indexing_dialog->exec() == QDialog::Rejected) {
        indexing_dialog->stopSearch();
    }
    delete indexing_dialog;

    end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    ui->statusBar->showMessage("The time: " + QString::number(elapsed_ms.count()) + " ms\n");
}

void MainWindow::find() {
    begin = std::chrono::steady_clock::now();

    auto *files_util_thread = new QThread();
    auto *files_util = new FilesUtil(index, ui->stringInput->text());
    files_util->moveToThread(files_util_thread);
    connect(files_util_thread, SIGNAL(started()), files_util, SLOT(findFilesWith()));
    connect(files_util, SIGNAL(filesWithStrFound(QVector<QString>)), files_util_thread, SLOT(quit()));
    connect(files_util, SIGNAL(filesWithStrFound(QVector<QString>)), files_util, SLOT(deleteLater()));
    connect(files_util, SIGNAL(filesWithStrFound(QVector<QString>)), this, SLOT(displayFilesWithStr(QVector<QString>)));
    connect(files_util_thread, SIGNAL(finished()), files_util_thread, SLOT(deleteLater()));
    files_util_thread->start();
}

void MainWindow::openFile(QTreeWidgetItem *item) {
    if (item != nullptr)
        QDesktopServices::openUrl(QUrl::fromLocalFile(item->text(1)));
}

void MainWindow::displayFilesWithStr(QVector<QString> files) {
    ui->filesWithStr->clear();
    for (auto &s : files) {
        auto *item = new QTreeWidgetItem(ui->filesWithStr);
        ui->filesWithStr->addTopLevelItem(item);
        QDir file(s);
        item->setText(0, file.dirName());
        item->setText(1, file.absolutePath());
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    ui->statusBar->showMessage("The time: " + QString::number(elapsed_ms.count()) + " ms, files: " + QString::number(files.size()));
}