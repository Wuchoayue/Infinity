#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QListView>
#include <QMenu>
#include "playlistmodel.h"

class PlayListView : public QListView
{
    Q_OBJECT
public:
    explicit PlayListView(QWidget *parent = 0);
    ~PlayListView();
    void insert();
    void on_customContextMenuRequested();

private:
    PlayListModel *playList_model;  //播放列表模型
    QMenu *playList_menu;    //选项菜单
    QAction *showMedia_action;   //查看音视频信息
    QAction *delMedia_action;   //删除选中的媒体
    QAction *playMedia_action;  //播放选中的媒体
};

#endif // PLAYLISTVIEW_H
