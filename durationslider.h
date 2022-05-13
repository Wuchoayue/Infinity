#ifndef DURATIONSLIDER_H
#define DURATIONSLIDER_H

#include <QSlider>
#include <QWidget>
#include <QMouseEvent>
#include <thumbnail.h>
#include <QTimer>

class DurationSlider : public QSlider
{
    Q_OBJECT
public:
    DurationSlider(QWidget * parent = 0);
    ~DurationSlider();
    Thumbnail* thumbnail = nullptr; //缩略图
    int thumbnail_y;
    QTimer* tm = nullptr;
signals:
    void sliderReleasedAt(int value);

public:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEnterEvent *event);
    void showThumbnail();
    bool isMoving;
    int m_value;
    bool mousePress;
};

#endif // DURATIONSLIDER_H
