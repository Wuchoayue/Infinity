#ifndef MYFULLSCREEN_H
#define MYFULLSCREEN_H

#include <QWidget>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QListWidget>
#include <QBoxLayout>
#include "windows.h"
#include "playercontrols.h"
#include "durationslider.h"

class myfullscreen : public QWidget
{
    Q_OBJECT

public:
    explicit myfullscreen(QWidget *parent = nullptr);
    //变为全屏
    void turnToFullScreen();
    //退出全屏
    void turnToNormal();
    //设置位置
    void setPosition();
    //转为不可见
    void turnToInvisable();

public:
    PlayerControls *getPc() const;
    QWidget *getVw() const;

signals:
    void on_clicked();

private:
    QWidget *vw = nullptr;  //视频窗口
    PlayerControls *pc = nullptr;   //控制模块
    bool isPcVisable = true;    //是否显示控制模块
    int pcLifeTime = 10;    //计数器
    QSize mysize;
    QTimer* tm = nullptr;   //控制模块定时器

protected:
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // MYFULLSCREEN_H
