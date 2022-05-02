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
    void sliderReleasedAt(int value);

public:  //mouse
    void mousePressEvent(QMouseEvent *event);  //单击
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool isMoving;
    int m_value;
    bool mousePress;
};

#endif // DURATIONSLIDER_H
