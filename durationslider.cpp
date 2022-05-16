#include "durationslider.h"
#include <QDebug>
#include <QEvent>

DurationSlider::DurationSlider(QWidget *parent)
{
    //加载qss样式文件
    QFile file(":/qss/durationSlider.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    setStyleSheet(stylesheet);
    file.close();

    m_value=0;
    isMoving = false;
    mousePress = false;
    thumbnail = new Thumbnail();
    tm = new QTimer();
    tm->setInterval(1000);
    connect(tm, &QTimer::timeout, this, &DurationSlider::showThumbnail);
}

DurationSlider::~DurationSlider()
{

}

void DurationSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
    isMoving = false;
    mousePress = true;
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value > maximum()) {
        value = maximum();
    }
    if(value < minimum()) {
        value = minimum();
    }
    m_value = value + 0.5;
    setValue(m_value);
}

void DurationSlider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value > maximum()){
        value = maximum();
    }
    if(value < minimum()){
        value = minimum();
    }

    thumbnail_x=event->globalX();

    if(parentWidget()->parentWidget()->isFullScreen()) {
        if(100 > thumbnail_x){
            thumbnail_x = 100;
        }
        if(parentWidget()->parentWidget()->width() - 100 < thumbnail_x){
            thumbnail_x = parentWidget()->parentWidget()->width() - 100;
        }
        thumbnail_y = parentWidget()->parentWidget()->height() - 20;
    }
    else {
        int tmp = parentWidget()->parentWidget()->parentWidget()->x();
        if(tmp + 100 > thumbnail_x) {
            thumbnail_x = tmp + 100;
        }
        if(tmp+parentWidget()->parentWidget()->width() - 100 < thumbnail_x) {
            thumbnail_x = tmp+parentWidget()->parentWidget()->width() - 100;
        }

        thumbnail_y = parentWidget()->parentWidget()->parentWidget()->y() + parentWidget()->parentWidget()->height();
    }
    thumbnail->setGeometry(thumbnail_x-100, thumbnail_y-190, 200, 150);
    if(mousePress) {
        m_value = value + 0.5;
        isMoving = true;
        thumbnail->setVisible(true);
        thumbnail->raise();
    }
    else {
        tm->start();
    }
}

void DurationSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);
    mousePress = false;
    isMoving = false;
    thumbnail->setVisible(false);
    emit sliderReleasedAt(m_value);
}

void DurationSlider::enterEvent(QEnterEvent *event)
{
    QSlider::enterEvent(event);
    setMouseTracking(true);
}

void DurationSlider::leaveEvent(QEvent *event)
{
    QSlider::leaveEvent(event);
    thumbnail->setVisible(false);
    setMouseTracking(false);
    tm->stop();
}

void DurationSlider::showThumbnail()
{
    thumbnail->setVisible(true);
    tm->stop();
}
