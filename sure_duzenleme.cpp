#include "sure_duzenleme.h"
#include "ui_sure_duzenleme.h"

QTableWidgetItem *itm;
int row;
int column;
QString dateConst;

Sure_Duzenleme::Sure_Duzenleme(QWidget *parent, QTableWidgetItem *item, double time, int quantity, QString date, QList<QTableWidget*> *twList) :
    QDialog(parent),
    ui(new Ui::Sure_Duzenleme)
{
    ui->setupUi(this);
    ui->label->setText("Barkod  :    " + item->text());
    ui->doubleSpinBox->setValue(time);
    QSqlQuery query;
    row = twList->indexOf(item->tableWidget());
    column = item->column();
    dateConst = date;
    query.exec("select aciklama from aciklama where row = " + QString::number(row)
               + " and column = " + QString::number(column)
               + " and tarih = '" + date + "'");
    query.next();
    ui->plainTextEdit->setPlainText(query.value(0).toString());
    ui->spinBox->setValue(quantity);
    itm = item;
}

Sure_Duzenleme::~Sure_Duzenleme()
{
    delete ui;
}

void Sure_Duzenleme::on_pushButton_clicked()
{
    QSqlQuery query;
    QSqlQuery qry;
    query.exec("update parca set time = " + QString::number(ui->doubleSpinBox->value()) + ", quantity = " + QString::number(ui->spinBox->value()) + " where barcode = '" + itm->text() + "'");
    itm->tableWidget()->setColumnWidth(itm->column(), 364 * ui->doubleSpinBox->value() / 225);
    query.exec("select aciklama from aciklama where row = " + QString::number(row) + " and column = " + QString::number(column) + " and tarih = '" + dateConst + "'");
    query.next();
    if(query.value(0).toString().isEmpty())
    {
        if(!ui->plainTextEdit->toPlainText().isEmpty())
        {
            qry.prepare("insert into aciklama values(:row,:column,:aciklama,:tarih)");
            qry.bindValue(":row", row);
            qry.bindValue(":column", column);
            qry.bindValue(":aciklama", ui->plainTextEdit->toPlainText());
            qry.bindValue(":tarih", dateConst);
            qry.exec();
        }
    }

    else
    {
        if(ui->plainTextEdit->toPlainText().isEmpty())
        {
            qry.exec("update aciklama set aciklama = '"
                     + QString(" ")
                     + "' where row = "
                     + QString::number(row)
                     + " and column = "
                     + QString::number(column)
                     + " and tarih = '"
                     + dateConst
                     + "'");
        }

        else
        {
            qry.exec("update aciklama set aciklama = '"
                     + ui->plainTextEdit->toPlainText()
                     + "' where row = "
                     + QString::number(row)
                     + " and column = "
                     + QString::number(column)
                     + " and tarih = '"
                     + dateConst
                     + "'");
        }
    }
    this->deleteLater();
}

