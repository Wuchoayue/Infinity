#include "myvideowidget.h"

MyVideoWidget::MyVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,
    QBrush(QPixmap("E:/videoplayer/资料/初步交互设计文档/播放.png").scaled( // 缩放背景图.
    QSize(635, 480),
    Qt::IgnoreAspectRatio,
    Qt::SmoothTransformation))); // 使用平滑的缩放方式
    setPalette(palette); // 至此, 已给widget加上了背景图.
    this->hide();

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
}

void MyVideoWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() <<rect();
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
