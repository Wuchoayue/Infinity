#include "myvideowidget.h"

MyVideoWidget::MyVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    qDebug() << rect();
    showMaximized();
    playControls_widget = new QWidget(this);
    playControls_widget->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    playControls_widget->setAttribute(Qt::WA_TranslucentBackground);
    playControls_widget->setStyleSheet("QWidget{color: #ffffff}");
    playerControls_timer = new QTimer(this);
    playerControls_timer->setInterval(5000);
    connect(playerControls_timer, &QTimer::timeout, this, [=] {
        playControls_widget->hide();
    });
}

MyVideoWidget::~MyVideoWidget()
{

}

void MyVideoWidget::init()
{
    playControls_widget->resize(width(), height() / 5);
    QPoint cur = mapToGlobal(QPoint(0, 0));
    playControls_widget->move(cur.x(), cur.y() + height() * 0.8);
    playControls_widget->show();
    qDebug() << rect();
}

void MyVideoWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "reszie";
    QPoint cur = mapToGlobal(QPoint(0, 0));
    playControls_widget->move(cur.x(), cur.y() + height() * 0.8);
    playControls_widget->resize(width(), height() / 5);
    return QWidget::resizeEvent(event);
}

//void MyVideoWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    playControls_widget->show();
//    playControls_widget->raise();
//    playerControls_timer->start();
//}
