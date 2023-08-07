#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QPushButton>

#include <QStandardItemModel>
#include <QTreeView>
#include <ctime>

#include <QTcpSocket>
#include <QTimer>
//文件操作
#include <QFileDialog>
#include <QMessageBox>
#include "mywritethread.h"
//数据库操作
#include <QSqlDatabase>
#include <QButtonGroup>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *leftLabel;
    QLabel *rightLabel1;
    QLabel *rightLabel2;
    QWidget *newPage; // 用于存储新页面的指针

    QStandardItemModel * model;
    void createTreeView();

    QTcpSocket *tcpSocket; //tcp套接字
    QTimer *timer; //定时器
    bool isConnected; //标记是否已连接到主机

    MyWriteThread *writeThread; //创建一个线程
    QString selectedFilePath;

    QSqlDatabase db;
    QButtonGroup *group;

    //数据库操作
    void createTable(); //创建数据库表
    void sortDataByRandNumber2(); //排序数据
    void printAndDeleteRowsWithRandNumber3(); //打印，删除

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void openNewPage(); //打开新页面
    void onPrintTriggered(); //触发打印事件
    void sortButtonClicked(); //表格倒序
    void setZero(); //设置为零
    void filterOddRows();  //过滤奇数行
    void startSendingData();
    void sendData();
    //文件操作
    void openFile(); //打开文件路径
    void onWriteFinished(); //写入完成
    //数据库操作
    void dbBtnsClickedSlot(int);


};

#endif // MAINWINDOW_H
