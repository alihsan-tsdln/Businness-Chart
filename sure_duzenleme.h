#ifndef SURE_DUZENLEME_H
#define SURE_DUZENLEME_H

#include <QDialog>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include <QSqlError>

namespace Ui {
class Sure_Duzenleme;
}

class Sure_Duzenleme : public QDialog
{
    Q_OBJECT

public:
    explicit Sure_Duzenleme(QWidget *parent = nullptr, QTableWidgetItem *item = nullptr, double time = 0, int quantity = 0 , QString date = "" , QList<QTableWidget*> *twList = nullptr);
    ~Sure_Duzenleme();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Sure_Duzenleme *ui;
};

#endif // SURE_DUZENLEME_H
