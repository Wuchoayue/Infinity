#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include "playlistview.h"

class PlayList : public QWidget
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = 0);
    ~PlayList();

public:


    PlayListView *getPlayList_listView() const;

private:
    PlayListView *playList_listView;
    QLabel *title_label;
    QLabel *num_label;
    QPushButton *clear_button;
};

#endif // PLAYLIST_H
