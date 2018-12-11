#include "indexingdialog.h"
#include "ui_indexingdialog.h"

IndexingDialog::IndexingDialog(QThread *searchThread, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::IndexingDialog),
        indexingThread(searchThread) {
    ui->setupUi(this);
    connect(ui->stopSearchButton, SIGNAL(clicked()), this, SLOT(stopSearch()));
    ui->progressBar->setValue(0);
}

IndexingDialog::~IndexingDialog() {
    delete ui;
}

void IndexingDialog::stopSearch() {
    this->done(0);
    indexingThread->requestInterruption();
}

void IndexingDialog::updateBar(int value) {
    ui->progressBar->setValue(value);
}


void IndexingDialog::setBarRange(int b) {
    ui->progressBar->setMaximum(b);
}