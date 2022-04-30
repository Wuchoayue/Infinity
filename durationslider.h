#ifndef DURATIONSLIDER_H
#define DURATIONSLIDER_H

#include <QSlider>
#include <QWidget>
#include <QMouseEvent>

class DurationSlider : public QSlider
{
    Q_OBJECT
public:
    DurationSlider(QWidget * parent = 0);
    ~DurationSlider();

signals:
    void sliderMoved(int);

public:  //mouse
//    void mousePressEvent(QMouseEvent *event);  //单击
//    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
//    bool m_isMoving;
//    int m_value;
//    bool m_mousePress;
};

#endif // DURATIONSLIDER_H
