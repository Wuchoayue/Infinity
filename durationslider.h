#ifndef DURATIONSLIDER_H
#define DURATIONSLIDER_H

#include <QSlider>
#include <QWidget>
#include <QMouseEvent>
#include <thumbnail.h>
#include <QTimer>
#include <QFile>

class DurationSlider : public QSlider
{
    Q_OBJECT

public:
    DurationSlider(QWidget * parent = 0);
    ~DurationSlider();

signals:
    void sliderReleasedAt(int value);

private:
    bool isMoving;
    int m_value;
    bool mousePress;
    Thumbnail* thumbnail = nullptr; //缩略图
    int thumbnail_x;
    int thumbnail_y;
    QTimer* tm = nullptr;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEnterEvent *event);
    void showThumbnail();
};

#endif // DURATIONSLIDER_H
