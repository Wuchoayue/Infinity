#include "durationslider.h"
#include <QDebug>
#include <QEvent>

DurationSlider::DurationSlider(QWidget *parent)
{
    m_value=0;
    isMoving = false;
    mousePress = false;
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
    if(value > maximum()){
        value = maximum();
    }
    if(value < minimum()){
        value = minimum();
    }
    if(mousePress) {
        m_value = value + 0.5;
        isMoving = true;
        emit sliderMoved(m_value);
    }
    setValue(value + 0.5);
}

void DurationSlider::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);
    mousePress = false;
    isMoving = false;
    emit sliderReleasedAt(m_value);
}
