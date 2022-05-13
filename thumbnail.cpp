#include "thumbnail.h"

Thumbnail::Thumbnail(QWidget *parent)
    : QWidget{parent}
{
     setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
     this->setVisible(false);
}
