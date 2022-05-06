#include "myvideowidget.h"

MyVideoWidget::MyVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    QPushButton *btn = new QPushButton("确定", this);
}

MyVideoWidget::~MyVideoWidget()
{

}


