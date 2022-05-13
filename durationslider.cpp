#include "durationslider.h"
#include <QDebug>
#include <QEvent>

DurationSlider::DurationSlider(QWidget *parent)
{
    m_value = 0;
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

void DurationSlider::mousePressEvent(QMouseEvent *event){
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

void DurationSlider::mouseMoveEvent(QMouseEvent *event){

    QSlider::mouseMoveEvent(event);
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value > maximum()) {
        value = maximum();
    }
    if(value < minimum()) {
        value = minimum();
    }
    thumbnail->setGeometry(event->globalX()-100, thumbnail_y-190,200,150);
    if(mousePress) {
        m_value = value + 0.5;
        isMoving = true;
        emit sliderMoved(m_value);
        thumbnail->setVisible(true);
    }
    else {
        tm->start();
    }
}

void DurationSlider::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);
    mousePress = false;
    isMoving = false;
    thumbnail->setVisible(false);
    emit sliderReleasedAt(m_value);
}

void DurationSlider::enterEvent(QEnterEvent *event){
    QSlider::enterEvent(event);
    setMouseTracking(true);
}

void DurationSlider::leaveEvent(QEvent *event){
    QSlider::leaveEvent(event);
    thumbnail->setVisible(false);
    setMouseTracking(false);
    tm->stop();
}

void DurationSlider::showThumbnail(){
    thumbnail->setVisible(true);
    tm->stop();
}
