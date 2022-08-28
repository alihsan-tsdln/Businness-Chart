#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDate>
#include <QFileDialog>
#include <QAxObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QListWidgetItem>
#include "sure_duzenleme.h"
#include "parca_ekle.h"
#include <QTimer>
#include <QtAlgorithms>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_tableWidget_cellChanged(int row, int column);

    void on_ekle_btn_clicked();

    void tableItemClicked(QTableWidgetItem* item);

    void on_parca_ekle_clicked();

    void listAdder();

    void on_parca_sil_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_template_olustur_clicked();

    void on_parca_sil_btn_clicked();

    void ItemClicked(QTableWidgetItem* item);

    void setMovement(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_excel_olustur_btn_clicked();

    void setMovementSec(QTableWidgetItem *item);

    void drawTimeLine();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_geri_btn_clicked();

    void on_ileri_btn_clicked();

    void on_listWidget_itemPressed(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QSqlDatabase sql = QSqlDatabase::addDatabase("QSQLITE");
    void init();
    void addItem(QTableWidget *tw, const QString barcode, int time);
    void tableWidgetAdder();
    void colorAdder();
    void listInit();
    void tabloUpdate();
    void addWeek();
    void addPastWeeks();
    void sortDays();
};
#endif // MAINWINDOW_H
