#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QTableWidget*> *twList;
QList<QBrush> *colorList;
QString filepath = "";
QStringList *alphabet;
QStringList *lst;
int fixedRow = 0;
QTableWidgetItem *prevItem = nullptr;
QLabel *labelLine = nullptr;

void MainWindow::init()
{
    if(sql.open())
        ui->statusbar->showMessage("Veritabanına Bağlanıldı");

    else
        ui->statusbar->showMessage("Veritabanına Bağlanılamadı");

    alphabet = new QStringList();
    alphabet->append("A");
    alphabet->append("B");
    alphabet->append("C");
    lst = new QStringList();
    lst->append("Barkod No");
    lst->append("Parça Adı");
    lst->append("Üretim Süresi (DK");


    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->listWidget->setDragEnabled(true);
    ui->tableWidget->setAcceptDrops(true);
    ui->tableWidget->setDefaultDropAction(Qt::MoveAction);
    ui->tableWidget->setDropIndicatorShown(true);
    fixedRow = ui->tableWidget->rowCount();

    ui->tableWidget->setHorizontalHeaderLabels(QStringList(QDate::currentDate().toString("dd.MM.yyyy")));

    tableWidgetAdder();
    colorAdder();
    listAdder();

    tabloHatirla();
    QTimer *timer = new QTimer(this);
    drawTimeLine();
    connect(timer, SIGNAL(timeout()), this, SLOT(drawTimeLine()));
    timer->start(60000);
}

void MainWindow::addItem(QTableWidget *tw, const QString barcode, int time)
{
    ui->tableWidget->setRowCount(fixedRow);
    if(prevItem)
    {
        QSqlQuery query;
        query.exec("delete from aciklama where row = "
                   + QString::number(twList->indexOf(prevItem->tableWidget()))
                   + " and column = "
                   + QString::number(prevItem->column()));
        prevItem->tableWidget()->removeColumn(prevItem->column());
        prevItem = nullptr;
    }
    tw->insertColumn(tw->columnCount());

    tw->setColumnWidth(tw->columnCount() - 1, 364 * time / 225);
    QTableWidgetItem *item = new QTableWidgetItem(barcode);
    QFont font = QFont();
    font.setBold(true);
    item->setFont(font);
    item->setBackground(colorList->at(rand() % colorList->count()));
    tw->setItem(0, tw->columnCount() - 1, item);
    ui->tableWidget->setRowCount(fixedRow);
}

void MainWindow::tableWidgetAdder()
{
    twList = new QList<QTableWidget*>();
    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidget *tw = new QTableWidget();
        tw->horizontalHeader()->hide();
        tw->verticalHeader()->hide();
        tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tw->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tw->insertRow(0);
        tw->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tw->setVerticalHeaderLabels(QStringList(QString::number(i)));
        tw->setDragEnabled(true);

        twList->append(tw);
        connect(twList->at(i), SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(tableItemClicked(QTableWidgetItem*)));
        connect(twList->at(i), SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(ItemClicked(QTableWidgetItem*)));
        ui->tableWidget->setCellWidget(i,0, twList->at(i));
    }


}

void MainWindow::colorAdder()
{
    colorList = new QList<QBrush>();
    colorList->append(QBrush(QColor::fromRgba(qRgba(255, 102, 102, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(255, 178, 102, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(255, 255, 102, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(178, 255, 102, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(102, 178, 255, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(102, 255, 0, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(255, 0, 127, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(76, 153, 0, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(255, 0, 0, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(0, 255, 0, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(0, 0, 255, 75))));
    colorList->append(QBrush(QColor::fromRgba(qRgba(0, 0, 204, 75))));
}

void MainWindow::listAdder()
{
    listInit();
    QSqlQuery query;
    query.exec("select barcode from parca");
    while(query.next())
    {
        ui->listWidget->addItem(query.value(0).toString());
    }
}

void MainWindow::listInit()
{
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        delete ui->listWidget->item(0);
    }
}

void MainWindow::tabloUpdate()
{
    QSqlQuery query;
    query.exec("delete from tablo");

    for(int i = 0; i < twList->count(); i++)
    {
        for(int j = 0; j < twList->at(i)->columnCount(); j++)
        {
            query.prepare("insert into tablo values(:tarih, :tezgah, :genislik, :barkod, :sira)");
            query.bindValue(":tarih", QDate::currentDate().toString("dd.MM.yyyy"));
            query.bindValue(":tezgah", i);
            query.bindValue(":genislik", 225 * twList->at(i)->columnWidth(j) / 364);
            query.bindValue(":barkod", twList->at(i)->item(0,j)->text());
            query.bindValue(":sira", j);

            if(!query.exec())
                qDebug() << query.lastError().text();
        }
    }
}

void MainWindow::drawTimeLine()
{
    if(labelLine)
    {
        labelLine->deleteLater();
        labelLine = nullptr;
    }
    int mesai = QTime::currentTime().addSecs(-30600).hour() * 60 + QTime::currentTime().addSecs(-30600).minute();
    if(mesai > 0 && mesai <= 450)
    {
        labelLine = new QLabel(this);
        QPixmap px(2,585);
        px.fill(Qt::black);
        labelLine->setPixmap(px);
        labelLine->setGeometry(205 + 14 * mesai / 9 ,25, 2, 585);
        labelLine->show();
    }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        if(ui->listWidget->item(i)->text().contains(arg1))
        {
            ui->listWidget->item(i)->setHidden(false);
        }

        else
        {
            ui->listWidget->item(i)->setHidden(true);
        }
    }
}


void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    ui->tableWidget->setRowCount(fixedRow);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if(row >= twList->count() || ui->tableWidget->item(row,column)->text().isEmpty())
        return;

    QSqlQuery query;
    query.exec("select time from parca where barcode = '" + ui->tableWidget->item(row,column)->text() + "'");
    query.next();

    addItem(twList->at(row), ui->tableWidget->item(row,column)->text(), query.value(0).toDouble());
    tabloUpdate();
    ui->tableWidget->item(row,column)->setText("");
}


void MainWindow::on_ekle_btn_clicked()
{
    filepath = QFileDialog::getOpenFileName(this,tr("Excel Ekle"), ".", tr( "Microsoft Office Excel (*.xls *.xlsx)"));

    ui->statusbar->showMessage("Yükleniyor.....");

    if(!filepath.isEmpty())
    {
        QAxObject *excel     = new QAxObject("Excel.Application");
        excel->dynamicCall("SetVisible (bool Visible)", false); //Do not display the form
        excel->setProperty("DisplayAlerts" , false);
        QAxObject *workbooks = excel->querySubObject("Workbooks");
        QAxObject *workbook  = workbooks->querySubObject("Open(const QString&)", filepath);
        QAxObject *sheets    = workbook->querySubObject("Worksheets");
        QAxObject *sheet     = sheets->querySubObject("Item(int)", 1);

        int r = 1;
        QAxObject *cell;
        QSqlQuery query;

        while(true)
        {
            QString X="A"+QString::number(++r);
            cell = sheet->querySubObject("Range(QVariant, QVariant)",X);
            if(cell->dynamicCall("Value()").toString().isEmpty())
                break;
            QString barcode = cell->dynamicCall("Value()").toString();
            X="C"+QString::number(r);
            cell = sheet->querySubObject("Range(QVariant, QVariant)",X);
            double time = cell->dynamicCall("Value()").toDouble();
            query.prepare("insert into parca values(:barcode,:time)");
            query.bindValue(":barcode", barcode);
            query.bindValue(":time", time);
            query.exec();
        }
        listAdder();
    }

    ui->statusbar->showMessage("Yüklendi");
}

void MainWindow::tableItemClicked(QTableWidgetItem *item)
{
    QSqlQuery query;
    query.exec("select time from parca where barcode = '" + item->text() + "'");
    query.next();
    Sure_Duzenleme *sd = new Sure_Duzenleme(this, item, query.value(0).toDouble(), twList);
    sd->show();
}

void MainWindow::on_parca_ekle_clicked()
{
    Parca_Ekle *pe = new Parca_Ekle();
    pe->show();
    connect(pe, SIGNAL(destroyed(QObject*)), this, SLOT(listAdder()));
}

QListWidgetItem *delItem = nullptr;

void MainWindow::on_parca_sil_clicked()
{
    if(delItem)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Uyarı", delItem->text() + " barkodlu parçayı silmek istiyor musunuz?", QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            QSqlQuery query;
            query.exec("delete from parca where barcode = '" + delItem->text() + "'");
            delete delItem;
            delItem = nullptr;
        }
    }
}


void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    delItem = item;
}


void MainWindow::on_template_olustur_clicked()
{
    ui->statusbar->showMessage("Yükleniyor......");
    QString filepath=QFileDialog::getSaveFileName( this ,tr("Template Oluştur"), "." ,tr( "Microsoft Office Excel (*.xlsx)" )); //Get the save path


    if (!filepath.isEmpty()){
            QAxObject *excel = new QAxObject(this);
            excel->setControl("Excel.Application"); //Connect to Excel control
            excel->dynamicCall("SetVisible (bool Visible)", false); //Do not display the form
            excel->setProperty("DisplayAlerts" , false); //Do not display any warning messages. If it is true, a prompt similar to "File has been modified, whether to save" will appear when closing
            QAxObject *workbooks = excel->querySubObject("WorkBooks"); //Get workbook collection
            workbooks->dynamicCall("Add"); //Create a new workbook
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook"); //Get the current workbook
            QAxObject *worksheets = workbook->querySubObject("Sheets"); //Get worksheet collection
            QAxObject *worksheet = worksheets->querySubObject("Item(int)" ,1); //Get worksheet 1 of the worksheet collection, namely sheet1
            QAxObject *cellX;

            for(int i = 0; i < alphabet->count(); i++)
            {
                QString X = alphabet->at(i) + QString::number(1);
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(lst->at(i)));
            }

            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//Save to filepath, be sure to use QDir::toNativeSeparators to convert the "/" in the path to "\", otherwise It must not be saved.
            workbook->dynamicCall("Close()");//Close the workbook
            excel->dynamicCall("Quit()");//Close excel
            delete excel;
            excel=NULL;

    }

    ui->statusbar->showMessage("Yüklendi",5);
}

QTableWidgetItem *delParca = nullptr;


void MainWindow::on_parca_sil_btn_clicked()
{
    if(delParca)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Uyarı", delParca->text() + " barkodlu parçayı silmek istiyor musunuz?", QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            QSqlQuery query;
            query.exec("delete from aciklama where row = "
                       + QString::number(twList->indexOf(delParca->tableWidget()))
                       + " and column = "
                       + QString::number(delParca->column()));
            delParca->tableWidget()->removeColumn(delParca->column());
            delParca = nullptr;
            tabloUpdate();
        }
    }
}

void MainWindow::ItemClicked(QTableWidgetItem *item)
{
    delParca = item;
    prevItem = item;
}

void MainWindow::setMovement(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    prevItem = current;
}


void MainWindow::on_excel_olustur_btn_clicked()
{
    ui->statusbar->showMessage("Yükleniyor......");
    QString filepath=QFileDialog::getSaveFileName( this ,tr("Excel Kaydet"), "." ,tr( "Microsoft Office Excel (*.xlsx)" )); //Get the save path

    if (!filepath.isEmpty()){
       QAxObject *excel = new QAxObject(this);
       excel->setControl("Excel.Application"); //Connect to Excel control
       excel->dynamicCall("SetVisible (bool Visible)", false); //Do not display the form
       excel->setProperty("DisplayAlerts" , false); //Do not display any warning messages. If it is true, a prompt similar to "File has been modified, whether to save" will appear when closing
       QAxObject *workbooks = excel->querySubObject("WorkBooks"); //Get workbook collection
       workbooks->dynamicCall("Add"); //Create a new workbook
       QAxObject *workbook = excel->querySubObject("ActiveWorkBook"); //Get the current workbook
       QAxObject *worksheets = workbook->querySubObject("Sheets"); //Get worksheet collection
       QAxObject *worksheet = worksheets->querySubObject("Item(int)" ,1); //Get worksheet 1 of the worksheet collection, namely sheet1
       QAxObject *cellX;

       lst->remove(1);
       alphabet->append("D");
       alphabet->append("E");
       lst->append("Tezgah Adı");
       lst->append("Tarih");
       lst->append("Açıklama");


       for(int i = 0; i < alphabet->count(); i++)
       {
           QString X = alphabet->at(i) + QString::number(1);
           cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
           cellX->dynamicCall("SetValue(const QVariant&)",QVariant(lst->at(i)));
       }

       lst->insert(1,"Parça Adı");
       lst->remove(2);
       lst->remove(2);
       lst->remove(2);

       int girdiCell = 1;

       for(int i = 0; i < twList->count(); i++)
       {
            for(int j = 0; j < twList->at(i)->columnCount(); j++)
            {
                QString X = alphabet->at(0) + QString::number(++girdiCell);
                QSqlQuery query;
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(twList->at(i)->item(0,j)->text()));
                X = alphabet->at(1) + QString::number(girdiCell);
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                query.exec("select time from parca where barcode = '" + twList->at(i)->item(0,j)->text() + "'");
                query.next();
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(query.value(0).toDouble()));
                X = alphabet->at(2) + QString::number(girdiCell);
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(ui->tableWidget->verticalHeaderItem(i)->text()));
                X = alphabet->at(3) + QString::number(girdiCell);
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(QDate::currentDate().toString("dd.MM.yyyy")));
                X = alphabet->at(4) + QString::number(girdiCell);
                query.exec("select aciklama from aciklama where row = "
                           + QString::number(i)
                           + " and column = "
                           + QString::number(j));
                query.next();
                cellX = worksheet->querySubObject("Range(QVariant, QVariant)",X);
                cellX->dynamicCall("SetValue(const QVariant&)",QVariant(query.value(0).toString()));
            }
       }

       workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//Save to filepath, be sure to use QDir::toNativeSeparators to convert the "/" in the path to "\", otherwise It must not be saved.
       workbook->dynamicCall("Close()");//Close the workbook
       excel->dynamicCall("Quit()");//Close excel
       delete excel;
       excel=NULL;

       ui->statusbar->showMessage("Yüklendi", 5);


    }
}

void MainWindow::tabloHatirla()
{
    QSqlQuery query;
    query.exec("select * from tablo");

    while(query.next())
    {
        if(query.value(0).toString() != QDate::currentDate().toString("dd.MM.yyyy"))
        {
            qDebug() << "Farklı gun";
        }
        //tw, barcode, time
        addItem(twList->at(query.value(1).toInt()), query.value(3).toString(), query.value(2).toDouble());
    }
}

void MainWindow::setMovementSec(QTableWidgetItem *item)
{
    prevItem = item;
}

