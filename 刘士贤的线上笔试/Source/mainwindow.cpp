#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //单击创建新页面
    connect(ui->newPage, SIGNAL(triggered()), this, SLOT(openNewPage()));
    //工具栏操作选项
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onPrintTriggered()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onPrintTriggered()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onPrintTriggered()));
    connect(ui->actionAllSave, SIGNAL(triggered()), this, SLOT(onPrintTriggered()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(onPrintTriggered()));

    //状态栏显示显示
    leftLabel = new QLabel(" 刘士贤的Qt界面程序", this);
    ui->statusBar->addWidget(leftLabel);

    rightLabel1 = new QLabel("线上笔试题  ", this);
    ui->statusBar->addPermanentWidget(rightLabel1);

    rightLabel2 = new QLabel(" 2023-8-7", this);
    ui->statusBar->addPermanentWidget(rightLabel2);

    //设置表格8行5列
    ui->tableWidget->setRowCount(8);
    ui->tableWidget->setColumnCount(5);

    //设置表头内容
    QStringList headerLabels;
    headerLabels << "字段1" << "字段2" << "字段3" << "字段4" << "字段5";
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    // 设置表格内容
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString("数据 %1").arg(row * ui->tableWidget->columnCount() + col + 1));
            //arg()函数将这个值插入到占位符 %1 的位置，得到最终的字符串。
            item->setTextAlignment(Qt::AlignCenter); // 设置文本居中对齐
            ui->tableWidget->setItem(row, col, item);
        }
    }

    ui->myButton->setText("倒序表格");
    ui->myButton->setFixedHeight(35);

    connect(ui->myButton,SIGNAL(clicked()),this,SLOT(sortButtonClicked()));
    // 7) finish-----------------------------------------------------

    createTreeView();
    connect(ui->setZeroBtn, SIGNAL(clicked()), this, SLOT(setZero()));
    connect(ui->filterBtn, SIGNAL(clicked()), this, SLOT(filterOddRows()));

    //13)
    ui->textShowArea->setReadOnly(true); //设置为只读
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(startSendingData()));

    tcpSocket = new QTcpSocket(this); //tcp套接字对象
    //设置定时器,每500毫秒触发一次sendData()槽函数
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(sendData()));

    //14)
    connect(ui->selectPathBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    writeThread = nullptr;

    //15)
    group = new QButtonGroup(this);
    group->addButton(ui->createTableBtn,1);
    group->addButton(ui->sortBtn,2);
    group->addButton(ui->printAndDeleteBtn,3);
    connect(group,SIGNAL(buttonClicked(int)),
            this,SLOT(dbBtnsClickedSlot(int)));

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydatabase.db");

    if(!db.open())
    {
        qDebug() << "数据库打开失败.";
    }
}

void MainWindow::sortButtonClicked()
{
    int rowCount = ui->tableWidget->rowCount();

    for (int i = 0; i < rowCount / 2; ++i)
    {
        ui->tableWidget->verticalHeader()->swapSections(i, rowCount - 1 - i);
        //交换表格（QTableWidget）中垂直表头（verticalHeader）的两个部分（sections），交换两行的位置。
        //swapSections(int first, int second),函数的第一个参数 first 是第一个部分的索引，第二个参数 second 是第二个部分的索引
    }
}

//适配tableWidget的宽度
void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    int numColumns = ui->tableWidget->columnCount();
    int tableWidth = ui->tableWidget->viewport()->width(); //获取可视区域的宽度
//    int tableHeight = ui->tableWidget->viewport()->height();

    int cellWidth = tableWidth / numColumns; //单元格宽度
//    int cellHeight = tableHeight / numRows;  //单元格高度

    for (int column = 0; column < numColumns; ++column) //设置列宽
    {
        ui->tableWidget->setColumnWidth(column, cellWidth);
    }
}

//创建树视图
void MainWindow::createTreeView()
{
    model = new QStandardItemModel(this);
    model->setColumnCount(5);

    srand(time(0));

    for (int r = 0; r < 8; ++r)
    {
        QStandardItem *parentItem = new QStandardItem("第 " + QString::number(r + 1) + " 行");

        for (int c = 0; c < 5; ++c)
        {
            int randNumber = qrand() % 51; // 生成50以内的随机数
            parentItem->appendRow(new QStandardItem(QString::number(randNumber)));
        }

        model->appendRow(parentItem);
    }

    // 隐藏头部视图
    ui->treeView->header()->setVisible(false);

    ui->treeView->setModel(model);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->treeView->setExpandsOnDoubleClick(true);
}

//新页面
void MainWindow::openNewPage()
{
    newPage = new QWidget;
    newPage->setAttribute(Qt::WA_DeleteOnClose); // 当关闭新页面时自动销毁
    newPage->setWindowTitle("新的页面");
    newPage->show();
}

void MainWindow::onPrintTriggered()
{
    //获取发送信号的操作选项
    QAction * action = qobject_cast<QAction*>(sender());

    if(action)
    {
        //获取操作选项名称
        QString actionName = action->text();
        //
        ui->myPrintText->clear();
        QString outputText = "您执行了 <" + actionName + "> 操作!!!";
        ui->myPrintText->appendPlainText(outputText);
    }
}

//设置为零操作
void MainWindow::setZero()
{
    for (int row = 0; row < model->rowCount(); ++row)
    {
        QModelIndex parentIndex = model->index(row, 0);
        if (parentIndex.isValid()) // 确保是父项
        {
            for (int childRow = 0; childRow < model->rowCount(parentIndex); ++childRow)
            {
                QModelIndex childIndex = model->index(childRow, 0, parentIndex);
                if (model->data(childIndex).toInt() <= 25)
                {
                    model->setData(childIndex, 0); // 设置子项为0
                }
            }
        }
    }
}

//过滤奇数行操作
void MainWindow::filterOddRows()
{
    QModelIndex rootIndex = ui->treeView->rootIndex(); //获取树形视图的根索引
    for (int row = model->rowCount(rootIndex)-1; row >= 0; --row) //从最后一行开始遍历并处理每一行
    {
        QModelIndex index = model->index(row, 0, rootIndex); //来获取指定行和第一列的索引
        if (index.row() % 2 != 0)
        {
            model->removeRow(row, rootIndex);
        }
    }
}

void MainWindow::startSendingData()
{
    // 点击发送数据按钮后，连接到本机的8888端口
    tcpSocket->connectToHost("127.0.0.1", 8888);

    // 启动定时器，每500ms发送一次数据
    timer->start(500);
}

//发送数据
void MainWindow::sendData()
{
    //模拟一个任意的字符串
    QString data = "This is a test text.\n";

    tcpSocket->write(data.toUtf8());

    //在文本显示的区域显示发送的内容
    ui->textShowArea->appendPlainText(data);
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getExistingDirectory(this, "选择文件路径", QDir::homePath());
    if(!filePath.isEmpty())
    {
        selectedFilePath = filePath + "/hello.txt";

        if(writeThread && writeThread->isRunning())
        {
            QMessageBox::warning(this, "正在写入", "请等当前的写入操作完成后再选择新的文件路径。");
            return;
        }

        writeThread = new MyWriteThread(selectedFilePath, this);

        //连接线程类的finishedWriting信号到onWriteFinished()槽函数
        connect(writeThread, SIGNAL(finishedWriting()), this, SLOT(onWriteFinished()));

        writeThread->start();
    }
}

void MainWindow::onWriteFinished()
{
    // 写入完成时的槽函数, 显示提示框, 释放线程类
    QMessageBox::information(this, "写入完成", "文件写入完成。");
    writeThread->deleteLater();
    writeThread = nullptr;
}

void MainWindow::dbBtnsClickedSlot(int id)
{
    if(id == 1)
        createTable();
    else if(id == 2)
        sortDataByRandNumber2();
    else if(id == 3)
        printAndDeleteRowsWithRandNumber3();
}

void MainWindow::createTable()
{
    //建表语句
    QString sql("CREATE TABLE databaseTable1(\
                    序号    INTEGER PRIMARY KEY,\
                    随机数1 INTEGER,\
                    随机数2 INTEGER,\
                    随机数3 INTEGER);");
    QSqlQuery sq; //数据库操作
    //执行sql语句
    if(sq.exec(sql))
    {
        qDebug() << "建表成功！";
        QMessageBox::information(this, "成功", "创建表(databaseTable1)成功。正在插入数据中...");
    }
    else
    {
        //输出错误信息
        QSqlError err = sq.lastError();
        QString text = err.text();
        qDebug() << "建表失败！" << text;
        QMessageBox::warning(this,"提示","建表失败!"+text);
    }

    // 插入数据
    for (int i = 0; i < 1000; ++i)
    {
        int randNumber1 = qrand() % 3000;
        int randNumber2 = qrand() % 3000;
        int randNumber3 = qrand() % 3000;

        if (!sq.exec(QString("INSERT INTO databaseTable1 (随机数1, 随机数2, 随机数3) VALUES (%1, %2, %3)").arg(randNumber1).arg(randNumber2).arg(randNumber3)))
        {
            qDebug() << "插入数据出错: " << sq.lastError().text();
            QMessageBox::warning(this,"提示","插入数据出错!"+sq.lastError().text());
            return;
        }
    }

    QMessageBox::information(this, "成功", "插入数据成功。");
    //插入数据后在界面显示
    if (!sq.exec("SELECT * FROM databaseTable1"))
    {
        qDebug() << "查询数据出错: " << sq.lastError().text();
        QMessageBox::warning(this,"提示","查询数据出错!"+sq.lastError().text());
        return;
    }

    QString output = "序号 随机数1 随机数2 随机数3\n";
    while (sq.next())
    {
        QString rowContent = QString(" %1,   %2,   %3,   %4")
                .arg(sq.value("序号").toInt())
                .arg(sq.value("随机数1").toInt())
                .arg(sq.value("随机数2").toInt())
                .arg(sq.value("随机数3").toInt());
        output += rowContent + "\n";
    }

//    QMessageBox::information(this, "排序结果", output);
    ui->dbDataShowArea->clear();
    ui->dbDataShowArea->appendPlainText(output);
}

void MainWindow::sortDataByRandNumber2()
{
    QSqlQuery sq;

    if (!sq.exec("SELECT * FROM databaseTable1 ORDER BY 随机数2 DESC"))
    {
        qDebug() << "排序数据出错: " << sq.lastError().text();
        QMessageBox::warning(this,"提示","排序数据出错!"+sq.lastError().text());
        return;
    }

    QString output = "按随机数2降序排序结果:\n序号 随机数1 随机数2 随机数3\n";
    while (sq.next())
    {
        QString rowContent = QString(" %1,   %2,   %3,   %4")
                .arg(sq.value("序号").toInt())
                .arg(sq.value("随机数1").toInt())
                .arg(sq.value("随机数2").toInt())
                .arg(sq.value("随机数3").toInt());
        output += rowContent + "\n";
    }

//    QMessageBox::information(this, "排序结果", output);
    ui->dbDataShowArea->clear();
    ui->dbDataShowArea->appendPlainText(output);
}

void MainWindow::printAndDeleteRowsWithRandNumber3()
{
    QSqlQuery sq;

    if (!sq.exec("SELECT * FROM databaseTable1 WHERE 随机数3 % 2 = 1"))
    {
        qDebug() << "查询数据出错: " << sq.lastError().text();
        QMessageBox::warning(this,"提示","查询数据出错!"+sq.lastError().text());
        return;
    }

    QString output = "随机数3为奇数的行内容:\n序号 随机数1 随机数2 随机数3\n";
    while (sq.next())
    {
        QString rowContent = QString(" %1,   %2,   %3,   %4")
                .arg(sq.value("序号").toInt())
                .arg(sq.value("随机数1").toInt())
                .arg(sq.value("随机数2").toInt())
                .arg(sq.value("随机数3").toInt());
        output += rowContent + "\n";
    }

    if (!sq.exec("DELETE FROM databaseTable1 WHERE 随机数3 % 2 = 0"))
    {
        qDebug() << "删除数据出错: " << sq.lastError().text();
        QMessageBox::warning(this,"提示","删除数据出错!"+sq.lastError().text());
        return;
    }

//    QMessageBox::information(this, "打印结果和删除成功", output);
    ui->dbDataShowArea->clear();
    ui->dbDataShowArea->appendPlainText(output);
}

MainWindow::~MainWindow()
{
    if (writeThread && writeThread->isRunning())
    {
        writeThread->stopWriting();
        writeThread->wait();
    }
    delete ui;
}
