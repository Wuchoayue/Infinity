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
#include "fullscreenplayercontrols.h"
#include "myvideowidget.h"
#include "player.h"
#include "windows.h"
#include "playercontrols.h"
#include "durationslider.h"
#include "qss.h"
class myfullscreen : public QWidget
{
    Q_OBJECT
public:
    explicit myfullscreen(QWidget *parent = nullptr);
    //初始化函数
    void init();
    //视频窗口
    MyVideoWidget* vw = nullptr;
    //控制栏
    PlayerControls*pc =nullptr;
    bool isPcVisable=true;
    int pcLifeTime = 10;
    //变为全屏
    void turnToFullScreen();
    //退出全屏
    void turnToNormal();
    QSize mysize;
    void resizeEvent(QResizeEvent* event);
    void mouseMoveEvent(QMouseEvent *e);

protected:
    QTimer* tm = nullptr;
    //设置位置
    void setPosition();
    //转为不可见
    void turnToInvisable();
signals:

};

#endif // MYFULLSCREEN_H
