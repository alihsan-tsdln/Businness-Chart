#ifndef PARCA_EKLE_H
#define PARCA_EKLE_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class Parca_Ekle;
}

class Parca_Ekle : public QDialog
{
    Q_OBJECT

public:
    explicit Parca_Ekle(QWidget *parent = nullptr);
    ~Parca_Ekle();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Parca_Ekle *ui;
};

#endif // PARCA_EKLE_H
