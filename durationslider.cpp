#include "durationslider.h"
#include <QDebug>
#include <QEvent>

DurationSlider::DurationSlider(QWidget *parent)
{
//    m_value=0;
}

DurationSlider::~DurationSlider()
{

}

//void DurationSlider::mousePressEvent(QMouseEvent *event){
//    QSlider::mousePressEvent(event);
//    double pos = event->pos().x() / (double)width();
//    double value = pos * (maximum() - minimum()) + minimum();
//    //value + 0.5 四舍五入
//    if(value>maximum()){
//        value=maximum();
//    }
//    if(value<minimum()){
//        value=minimum();
//    }
//    setValue(m_value);
//    emit sliderMoved(m_value);
//}
//void DurationSlider::mouseMoveEvent(QMouseEvent *event){
//    qDebug() << 1;
//    QSlider::mouseMoveEvent(event);
//    double pos = event->pos().x() / (double)width();
//    double value = pos * (maximum() - minimum()) + minimum();
//    if(value>maximum()){
//        value=maximum();
//    }
//    if(value<minimum()){
//        value=minimum();
//    }
//    //value + 0.5 四舍五入
//    if(m_mousePress) {
//        m_value=value + 0.5;
//        m_isMoving=true;
//        emit sliderMoved(m_value);
//    }
//    setValue(value + 0.5);
//}

void DurationSlider::mouseReleaseEvent(QMouseEvent *event){
    QSlider::mouseReleaseEvent(event);
    double pos = event->pos().x() / (double)width();
    double value = pos * (maximum() - minimum()) + minimum();
    if(value>maximum()){
        value=maximum();
    }
    if(value<minimum()){
        value=minimum();
    }
    setValue(value + 0.5);
    emit sliderMoved(value + 0.5);
}



