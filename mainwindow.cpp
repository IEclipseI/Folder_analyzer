#include <QDir>
#include <QFileDialog>
#include <iostream>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QHash>
#include <QtCore/QThread>
#include <QMetaType>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "searchdialog/searchdialog.h"
#include "searchdialog/ui_searchdialog.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->removeFromListButton->setEnabled(false);
    ui->directoryList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->directoryList->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    connect(ui->inputDirectoryName, SIGNAL(textChanged(const QString&)), this, SLOT(inputDirectoryNameTextChanged(const QString &)));
    connect(ui->chooseDirectoryButton, SIGNAL(clicked()), this, SLOT(chooseDir()));
    connect(ui->addToTrackButton, SIGNAL(clicked()), this, SLOT(addToSearch()));
    connect(ui->removeFromListButton, SIGNAL(clicked()), this, SLOT(removeFromList()));
    connect(ui->directoryList, SIGNAL(itemSelectionChanged()), this, SLOT(directoryListItemSelectionChanged()));
    connect(ui->addToTrackButton, SIGNAL(clicked()), this, SLOT(search()));
    int size = 400;
    ui->splitter->setSizes(QList<int>{ui->splitter->height() - size, size});
    connect(ui->find, SIGNAL(clicked()), this, SLOT(find()));
}

MainWindow::~MainWindow() {
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

void MainWindow::addToSearch() {
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
    item->setText(0, d.dirName());
    item->setText(1, d.absolutePath());
    ui->directoryList->addTopLevelItem(item);
    ui->inputDirectoryName->clear();
}

void MainWindow::removeFromList() {
    for (auto a : ui->directoryList->selectedItems()) {
        filesUtil.removeDirectory(a->text(1));
    }
    qDeleteAll(ui->directoryList->selectedItems());
}

void MainWindow::directoryListItemSelectionChanged() {
    ui->removeFromListButton->setEnabled(ui->directoryList->selectedItems().length() != 0);
}

void MainWindow::search() {
    for (int i = 0; i < ui->directoryList->topLevelItemCount(); i++) {
        filesUtil.addDirectory(ui->directoryList->topLevelItem(i)->text(1));
    }
}

void MainWindow::find() {
    ui->duplicates->clear();
    QString str = ui->stringInput->text();
    QVector<QString> filesWithStr = filesUtil.findFilesWith(str);
    for (auto &s : filesWithStr) {
        auto *item = new QTreeWidgetItem(ui->duplicates);
        ui->duplicates->addTopLevelItem(item);
        QDir file(s);
        item->setText(0, file.absolutePath());
        item->setText(1, file.absolutePath());
    }
}