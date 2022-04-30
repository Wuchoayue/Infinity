#include "playlistview.h"

PlayListView::PlayListView(QWidget *parent)
{
    playList_model = new PlayListModel(this);
    this->setModel(playList_model);
    setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    playList_menu = new QMenu(this);
    playMedia_action = new QAction(tr("播放"), playList_menu);
    QAction *addMedia_action = new QAction(tr("添加"), playList_menu);
    delMedia_action = new QAction(tr("删除"), playList_menu);
    showMedia_action = new QAction(tr("详细信息"), playList_menu);

    //显示菜单
//    connect(this, &PlayListView::customContextMenuRequested, playList_menu, &PlayListView::on_customContextMenuRequested);
    //播放
    //    connect(playMedia_action, &QAction::triggered, this, [=] {
    //        QModelIndex =
    //    });
}

PlayListView::~PlayListView()
{

}

void PlayListView::insert()
{
    playList_model->insert(QUrl("E:/CourseProject/test.png"));
}

void PlayListView::on_customContextMenuRequested()
{
//    bool isEnable = !selectionModel()->selectedIndexes().isEmpty();
//    showMedia_action->setEnabled(isEnable);
//    delMedia_action->setEnabled(isEnable);
//    playMedia_action->setEnabled(isEnable);

//    m_menu->exec(QCursor::pos());
}
