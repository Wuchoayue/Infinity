#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QListView>
#include <QMenu>
#include <QFileDialog>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QSqlQuery>
#include <QPushButton>
#include <QMessageBox>
#include "playlistmodel.h"

class PlayListView : public QListView
{
    Q_OBJECT

public:
    enum PlayMode {
        CurrentItemOnce, //只播放当前
        Sequential, //顺序播放
        Loop, //循环播放
        Random //随机播放
    };
    explicit PlayListView(QWidget *parent = 0);
    ~PlayListView();
    void loadPlayList(QSqlQuery *infinityPlayer_sqlQuery);    //加载数据表
    void savePlayList(QSqlQuery *infinityPlayer_sqlQuery);    //保存数据表
    void insert(const QUrl &path);
    void on_customContextMenuRequested(const QPoint &pos);
    void on_delMedia(); //删除音视频
    void on_addMedia(); //添加音视频
    void on_showMedia(); //查看音视频信息
    void preOne(QList<QString> &playHistory, int &curPlayHistorys);  //上一首
    void nextOne(); //下一首
    void changePlayMode(int value); //改变播放模式
    int playListNum();  //播放列表数目
    void normalNextOne();   //自动播放下一首
    int totalMedia();
    void clearMedia();  //清空播放列表
    bool existMedia(QString path);  //判断是否存在某个路径
    bool canPlay(QModelIndex index); //判断是否可以播放
    bool isFileExist(QString fileName); //判断文件是否存在
    void removeOne(QModelIndex index);  //删除一个播放列表项
    bool isAudio(QString path); //判断是不是音频

signals:
    void changeMedia(QString path); //改变播放对象
    void preMedia(QString path);    //上一首
    void noMedia(); //播放列表无音视频
    void haveMedia();   //播放列表有音视频
    void changePlayList();  //播放列表条目改变

private:
    PlayListModel *playList_model;  //播放列表模型
    QString currentPath = "";   //当前正在播放的音视频
    PlayMode currentPlayMode = PlayMode::Sequential;    //播放模式
};

#endif // PLAYLISTVIEW_H
