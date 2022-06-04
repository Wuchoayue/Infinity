#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>
#include <QBoxLayout>
#include <QMediaPlayer>
#include <QStyle>
#include <QTimer>
#include <QMenu>
#include "durationslider.h"

QT_BEGIN_NAMESPACE
class QToolButton;
class QSlider;
class QComboBox;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControls(QWidget *parent = nullptr);
    //框架
    QHBoxLayout *progressBar_layout = nullptr;  // 放进度条

public:
    QToolButton *getChangeMediaDirShow_button() const;
    QToolButton *getPlayStatus_button() const;
    QToolButton *getPreOne_button() const;
    QToolButton *getNextOne_button() const;
    QToolButton *getVolume_button() const;
    QToolButton *getFullScreen_button() const;
    QToolButton *getShowList_button() const;
    QLabel *getDuration_label() const;
    DurationSlider *getDuration_slider() const;
    QSlider *getVolume_slider() const;
    QWidget *getVolumeControl() const;
    QToolButton *getVolumeShow_button() const;
    QToolButton *getVolumeGraphy_button() const;

signals:
    void changeMediaDirShow_signal(); //改变目录显示状态
    void durationStep_signal(int);  //进度变化幅度
    void playMode_signal(int);  //播放模式
    void preOne_signal();   //上一首
    void playStatus_signal();   //播放状态
    void nextOne_signal();  //下一首
    void playSpeed_signal(double); //播放倍速
    void volume_signal(int);    //音量控制
    void volumeGraphy_signal(); //音频波形图
    void fullScreen_signal();    //改变屏幕大小
    void showList_signal(); //显示列表

private:
    QToolButton *changeMediaDirShow_button = nullptr;   //改变目录显示状态
    QPushButton *durationStep_button = nullptr;    //进度改变幅度
    int durationStep_value = 4; //进度改变幅度
    QToolButton *playMode_button = nullptr; //播放模式控制
    int currentPlayMode = 1;   //当前播放模式
    QPushButton *playSpeed_button = nullptr;    //播放速度控制
    double currentPlaySpeed = 1;   //当前播放倍速
    QToolButton *playStatus_button = nullptr; //控制播放状态
    QToolButton *preOne_button = nullptr; //控制上一首
    QToolButton *nextOne_button = nullptr;    //控制下一首
    QToolButton *volumeShow_button = nullptr;   //音量控制显示
    QWidget *volumeControl = nullptr;   //音量控制模块
    QToolButton *volume_button = nullptr;   //音量按钮
    QSlider *volume_slider = nullptr;   //音量滑杆
    bool isMuted = false; //是否静音
    int volume_int = 50; //音量
    QToolButton *volumeGraphy_button = nullptr; //波形图
    QToolButton *fullScreen_button = nullptr;   //改变屏幕大小
    QToolButton *showList_button = nullptr; //显示隐藏列表
    QLabel* duration_label = nullptr;
    DurationSlider* duration_slider = nullptr;
};

#endif // PLAYERCONTROLS_H
