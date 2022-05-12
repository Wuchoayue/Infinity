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
    PlayListView *playList_listView;

private:
    QLabel *title_label;
    QLabel *num_label;
    QPushButton *clear_button;
};

#endif // PLAYLIST_H
