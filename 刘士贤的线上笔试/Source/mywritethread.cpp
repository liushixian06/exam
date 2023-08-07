#include "mywritethread.h"

MyWriteThread::MyWriteThread(const QString& filePath,QObject *parent) :
    QThread(parent), filePath(filePath)
{
    this->stopFlag = false;
}

void MyWriteThread::stopWriting()
{
    stopFlag = true;
}

void MyWriteThread::run()
{
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) //没能成功打开
    {
        emit finishedWriting();
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8"); // 设置字符编码为UTF-8，确保文件编码与字符编码匹配
    for(int i = 0; i < 100 && !stopFlag; ++i)
    {

        QString data = "你好,\"文件\"\n"; // 任意字符串，包含中文和特殊字符
        stream << data.toUtf8(); // 将字符串转换为UTF-8编码的字节流并写入文件
        stream.flush(); //刷新
        msleep(500);
    }

    file.close();
    emit finishedWriting();
}
