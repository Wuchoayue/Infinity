#ifndef INFINITYPLAYER_H
#define INFINITYPLAYER_H

#include <QWidget>
#include <QListView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QFileDialog>
#include <QStyle>
#include <QListWidget>
#include <string.h>
#include <QKeyEvent>
#include <QTimer>
#include <QHash>
#include "playlistview.h"
#include "playercontrols.h"
#include "player.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QComboBox;
class QSlider;
class QStatusBar;
class QVideoWidget;
class QLineEdit;
class QToolButton;
class QLineEdit;
class PlayListModel;
QT_END_NAMESPACE

class InfinityPlayer : public QWidget
{
    Q_OBJECT

public:
    InfinityPlayer(QWidget *parent = nullptr);
    ~InfinityPlayer();
    void loadMediaDir();    //加载媒体库目录
    //添加目录
    void on_addMediaDir_button_clicked();
    void addMediaDir();
    //删除目录
    void on_delMediaDir_button_clicked();
    //重命名目录
    void changeMediaDir();
    //显示目录项
    void showMediaItem(QListWidgetItem *item);
    //添加目录项
    void on_addMediaItem_button_clicked();
    //删除目录项
    void on_delMediaItem_button_clicked();
    void on_mediaDir_menu(const QPoint &pos);
    //改变目录界面显示状态
    void changeMediaDirShow();
    //播放
    void playMedia(QString path);
    //保存播放列表
    void on_savePlayList();
    //将秒数转换成时间格式
    QString secTotime(int second);
    void initPlay();    //播放前初始化
    void on_preMedia(QString path);
    void nextMedia();   //播放下一个音视频

private:
    QSqlDatabase infinityPlayer_dataBase;  //数据库
    QSqlQuery *infinityPlayer_sqlQuery;     //数据库Sql
    QListWidget *mediaDir_listWidget;    //媒体库目录
    QLabel *mediaDir_label;
    QToolButton *addMediaDir_button;    //添加目录按钮
    QToolButton *delMediaDir_button;    //删除目录按钮
    QLineEdit *dirName_lineEdit;    //添加目录
    QLineEdit *dirName_lineEdit_1;  //修改目录
    QTableView *mediaItem_tableView; //目录项
    QLabel *mediaItem_label;
    QToolButton *addMediaItem_button;    //添加目录项按钮
    QToolButton *delMediaItem_button;    //删除目录项按钮
    QSqlQueryModel mediaItem_sqlQueryModel;  //存放目录查询结果
    bool isMediaDirShow = false;    //当前是否打开了目录
    PlayerControls *playerControls;     //控制部分
    QVideoWidget *video_videoWidget;    //播放器视频通道
    Player *player;   //后端播放模块
    bool isPlay = false;    //是否在播放
    double mediaDuration = -1;   //音视频全长
    QString mediaDuration_str = "";  //音视频全长字符串
    DurationSlider *duration_slider = nullptr;  //进度滑杆
    QTimer *duration_timer; //进度定时器
    QLabel *duration_label; //进度显示
    PlayListView *playList_listView;  //播放列表
    QString currentPath = "";    //当前播放的视频
    QSet<QString> supportType; //支持的文件类型
    QList<QString> playHistory; //历史播放队列
    int curPlayHistory = -1; //当前处于历史播放队列的位置
    int currentPlaySpeed = 1;   //当前播放速度
    int currentVolume = 50; //当前音量大小

protected:
    void keyPressEvent(QKeyEvent *event);   //键盘事件
    void dragEnterEvent(QDragEnterEvent *event);    //拖动文件进入窗口触发
    void dropEvent(QDropEvent *event);  //释放文件触发
};
#endif // INFINITYPLAYER_H
