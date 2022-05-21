#ifndef INFINITYPLAYER_H
#define INFINITYPLAYER_H

#include <QWidget>
#include "playlist.h"
#include "mediadir.h"
#include "myfullscreen.h"

class InfinityPlayer : public QWidget
{
    Q_OBJECT

public:
    InfinityPlayer(QWidget *parent = nullptr);
    ~InfinityPlayer();
    void loadMediaDir();    //加载媒体库目录
    //改变目录界面显示状态
    void changeMediaDirShow();
    //播放
    void playMedia(QString path);
    //将秒数转换成时间格式
    QString secTotime(int second);
    void initPlay();    //播放前初始化
    void on_preMedia(QString path);
    void nextMedia();   //播放下一个音视频
    PlayerControls *getPlayerControls() const;

private:
    QSqlDatabase infinityPlayer_dataBase;  //数据库
    QSqlQuery *infinityPlayer_sqlQuery;     //数据库Sql
    MediaDir *mediaDir; //媒体库
    bool isMediaDirShow = false;    //当前是否打开了目录
    PlayerControls *playerControls;     //控制部分
    myfullscreen *video;   //播放模块
    Player *player;   //后端播放模块
    bool isPlay = false;    //是否在播放
    double mediaDuration = -1;   //音视频全长
    QString mediaDuration_str = "";  //音视频全长字符串
    DurationSlider *duration_slider = nullptr;  //进度滑杆
    QTimer *duration_timer; //进度定时器
    QLabel *duration_label; //进度显示
    PlayList *playList = nullptr;   //播放列表
    QSet<QString> supportType; //支持的文件类型
    QList<QString> playHistory; //历史播放队列
    int curPlayHistory = -1; //当前处于历史播放队列的位置
    double currentPlaySpeed = 1;   //当前播放速度
    int currentVolume = 50; //当前音量大小
    bool isShowList = true;
    int currentDurationStep = 4;
    double currentDuration = 0;

protected:
    void keyPressEvent(QKeyEvent *event);   //键盘事件
    void dragEnterEvent(QDragEnterEvent *event);    //拖动文件进入窗口触发
    void dropEvent(QDropEvent *event);  //释放文件触发

};
#endif // INFINITYPLAYER_H
