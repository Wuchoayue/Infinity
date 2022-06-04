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
    Thumbnail* thumbnail = nullptr; //缩略图
    void setIsVideo(bool newIsVideo);

signals:
    void sliderReleasedAt(int value);
    void s_valueSignal(int s_value);

private:
    bool isMoving;
    int m_value;
    int s_value;
    bool mousePress;

    int thumbnail_x;
    int thumbnail_y;
    QTimer* tm = nullptr;
    bool isVideo = false;   //正在播放的是不是视频

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEnterEvent *event);
    void showThumbnail();
};

#endif // DURATIONSLIDER_H
