#include "thumbnail.h"

Thumbnail::Thumbnail(QWidget *parent)
    : QWidget{parent}
{
     setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
     this->setVisible(false);
     pixmap = new QPixmap();
     pic = new QLabel(this);
//     pic->resize(size());
     pic->setGeometry(0,0,200,110);
     time = new QLabel(this);
     time->setGeometry(0,110,200,20);
     time->setAlignment(Qt::AlignCenter);
}
