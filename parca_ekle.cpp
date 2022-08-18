#include "parca_ekle.h"
#include "ui_parca_ekle.h"

Parca_Ekle::Parca_Ekle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Parca_Ekle)
{
    ui->setupUi(this);
}

Parca_Ekle::~Parca_Ekle()
{
    delete ui;
}

void Parca_Ekle::on_pushButton_clicked()
{
    QSqlQuery query;
    query.prepare("insert into parca values(:barcode, :time)");
    query.bindValue(":barcode", ui->lineEdit->text());
    query.bindValue(":time", ui->doubleSpinBox->value());

    if(!query.exec())
    {
        qDebug() << query.lastError().text();
        if(query.lastError().nativeErrorCode() == "1555")
        {
            QMessageBox::information(this, "Parça var.", "Eklemek istediğiniz parça bilgileri mevcuttur.");
        }
    }

    this->deleteLater();
}

