#include "playlist.h"

PlayList::PlayList(QWidget *parent)
    : QWidget(parent)
{
    //控件定义
    playList_listView = new PlayListView(this);
    title_label = new QLabel("播放列表", this);
    num_label = new QLabel("0个资源", this);
    clear_button = new QPushButton("清空", this);

    //设置布局
    QVBoxLayout *left_layout = new QVBoxLayout;
    left_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->addWidget(title_label);
    left_layout->addWidget(num_label);

    QHBoxLayout *top_layout = new QHBoxLayout;
    left_layout->setContentsMargins(0, 0, 0, 0);
    top_layout->addLayout(left_layout);
    top_layout->addSpacing(10);
    top_layout->addWidget(clear_button);

    QVBoxLayout *layout = new QVBoxLayout;
    left_layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(top_layout, 1);
    layout->addWidget(playList_listView, 8);

    setLayout(layout);

    //信号与槽函数
    //播放列表数目改变
    connect(playList_listView, &PlayListView::changePlayList, this, [=] {
        qDebug() << playList_listView->playListNum();
        num_label->setText(QString::number(playList_listView->playListNum()) + "个资源");
    });
    //清空播放列表
    connect(clear_button, &QPushButton::clicked, playList_listView, &PlayListView::clearMedia);
}

PlayList::~PlayList()
{

}
