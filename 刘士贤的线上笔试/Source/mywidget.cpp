#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent) :
    QWidget(parent)
{
}

void MyWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); //消除编译器产生的"未使用参数"的警告。

    QPainter painter(this);
    QPixmap image(":/new/prefix1/img/image.png");

    //将图片绘制在控件的中心位置
    int x = (this->width() - image.width()) / 2;
    int y = (this->height() - image.height()) / 2;
    painter.drawPixmap(x, y, image);
}

