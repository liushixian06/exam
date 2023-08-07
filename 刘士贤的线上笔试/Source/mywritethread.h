#ifndef MYWRITETHREAD_H
#define MYWRITETHREAD_H

#include <QThread>
#include <QFile>
#include <QTextStream>

class MyWriteThread : public QThread
{
    Q_OBJECT
public:
    explicit MyWriteThread(const QString& filePath,QObject *parent = 0);
    void stopWriting();

signals:
    void finishedWriting();

protected:
    void run();

private:
    QString filePath;
    bool stopFlag;

public slots:

};

#endif // MYWRITETHREAD_H
