#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent * event) override;

signals:

public slots:

};

#endif // MYWIDGET_H
