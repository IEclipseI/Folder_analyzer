#ifndef STOPSEARCH_H
#define STOPSEARCH_H

#include <QDialog>
#include <QThread>

namespace Ui {
class IndexingDialog;
}

class IndexingDialog : public QDialog
{
    Q_OBJECT

public:
    IndexingDialog(QThread* searchThread, QWidget *parent = 0);
    ~IndexingDialog();

public slots:
    void stopSearch();
    void updateBar(int);
    void setBarRange(int);
private:
    Ui::IndexingDialog *ui;
    QThread* indexingThread;
};

#endif // STOPSEARCH_H
