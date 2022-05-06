#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <QVideoWidget>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QBoxLayout>
#include <QPainter>

class MyVideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit MyVideoWidget(QWidget *parent = 0);
    ~MyVideoWidget();

};

#endif // MYVIDEOWIDGET_H
