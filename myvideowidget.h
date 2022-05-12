#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <QVideoWidget>
#include <QPushButton>
#include <QWidget>
#include <QDebug>
#include <QBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include "playercontrols.h"

class MyVideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    explicit MyVideoWidget(QWidget *parent = 0);
    ~MyVideoWidget();
    void init();

public:
    QWidget *playControls_widget;
    QTimer *playerControls_timer; //控制模块的显示与隐藏

protected:
    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MYVIDEOWIDGET_H
