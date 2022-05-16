#include "playlistview.h"

PlayListView::PlayListView(QWidget *parent) : QListView(parent)
{
    playList_model = new PlayListModel(this);
    setModel(playList_model);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection); //设置选中多行
    setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑

    //显示菜单
    connect(this, &PlayListView::customContextMenuRequested, this, &PlayListView::on_customContextMenuRequested);
    //双击播放
    connect(this, &PlayListView::doubleClicked, this, [=] {
        QModelIndex index = currentIndex();
        int old = playList_model->rowOfPath(currentPath);
        playList_model->item(old)->setForeground(QBrush(QColor(0, 0, 0)));
        currentPath = playList_model->media(index);
        playList_model->item(index.row())->setForeground(QBrush(QColor(255, 0, 0)));
        emit changeMedia(currentPath);
    });
    //播放列表数目改变
    connect(playList_model, &PlayListModel::changePlayList, this, [=] {
        emit changePlayList();
    });
}

PlayListView::~PlayListView()
{

}

void PlayListView::loadPlayList(QSqlQuery *infinityPlayer_sqlQuery)
{
    //创建文件夹存放图标
    QDir *dir = new QDir;
    if(!dir->exists("../PlayListIcon")) {
        dir->mkpath("../PlayListIcon");
    }
    QString sql = "SELECT * FROM PlayList";
    if(infinityPlayer_sqlQuery->exec(sql)) {
        while(infinityPlayer_sqlQuery->next()) {
            playList_model->insertAll(infinityPlayer_sqlQuery->value(0).toString(), infinityPlayer_sqlQuery->value(1).toString());
        }
    }
}

void PlayListView::savePlayList(QSqlQuery *infinityPlayer_sqlQuery)
{
    infinityPlayer_sqlQuery->exec("DELETE FROM PlayList");
    QList<Element> pathes = playList_model->totalMedia();
    for(int i=0; i<pathes.size(); i++) {
        infinityPlayer_sqlQuery->exec(QString("INSERT INTO PlayList VALUES('%1', '%2')").arg(pathes[i].path).arg(pathes[i].cover));
    }
}

//插入音视频
void PlayListView::insert(const QUrl &path)
{
    playList_model->insert(path);
}

//右键产生菜单
void PlayListView::on_customContextMenuRequested(const QPoint &pos)
{
    std::shared_ptr<QMenu> menu = std::make_shared<QMenu>();    //选项菜单
    QAction *playMedia_action = menu->addAction(tr("播放"));  //播放选中的媒体
    QAction *delMedia_action = menu->addAction(tr("删除"));   //删除选中的媒体
    QAction *addMedia_action = menu->addAction(tr("添加"));    //添加音视频
    QAction *showMedia_action = menu->addAction(tr("详细信息"));   //查看音视频信息

    //如果没有选中项目，将部分设为不可用
    bool isEnable = !selectionModel()->selectedIndexes().isEmpty();
    playMedia_action->setEnabled(isEnable);
    delMedia_action->setEnabled(isEnable);
    showMedia_action->setEnabled(isEnable);

    if(isEnable) {
        QString path = playList_model->media(selectionModel()->selectedIndexes()[0]);
        if(currentPath == path) delMedia_action->setEnabled(false);
    }

    //播放
    connect(playMedia_action, &QAction::triggered, this, [=] {
        QModelIndex index = currentIndex();
        int old = playList_model->rowOfPath(currentPath);
        playList_model->item(old)->setForeground(QBrush(QColor(0, 0, 0)));
        currentPath = playList_model->media(index);
        playList_model->item(index.row())->setForeground(QBrush(QColor(255, 0, 0)));
        emit changeMedia(currentPath);
    });
    //删除音视频
    connect(delMedia_action, &QAction::triggered, this, &PlayListView::on_delMedia);
    //添加音视频
    connect(addMedia_action, &QAction::triggered, this, &PlayListView::on_addMedia);
    //查看音视频信息
    connect(showMedia_action, &QAction::triggered, this, &PlayListView::on_showMedia);

    menu->exec(QCursor::pos());
}

//删除音视频
void PlayListView::on_delMedia()
{
    QList<QModelIndex> indexes = selectedIndexes();
    playList_model->remove(indexes);
    if(playList_model->rowCount() == 0) emit noMedia();
}

//添加音视频
void PlayListView::on_addMedia()
{
    if(playList_model->rowCount() == 0) {
        emit haveMedia();
    }
    QUrl path = QFileDialog::getOpenFileName(this,"open file",".","Audio (*.mp3 *.wav);;Video (*.mp4 *.avi *.flv)");
    if(!path.isEmpty()) {
        insert(path);
    }
}

//显示音视频信息
void PlayListView::on_showMedia()
{
    QModelIndex index = currentIndex();
    playList_model->showMedia(index);
}

//上一首
void PlayListView::preOne(QList<QString> &playHistory, int &curPlayHistory)
{
    //只有播放列表不为空才有上一首
    if(playList_model->rowCount() != 0) {
        //上一首就是播放列表的上一首
        if(currentPlayMode < 3) {
            QModelIndex index = currentIndex();
            playList_model->item(index.row())->setForeground(QBrush(QColor(0, 0, 0)));
            if(index.row() > 0) {
                setCurrentIndex(playList_model->index(index.row()-1, 0));
            }
            else {
                setCurrentIndex(playList_model->index(playList_model->rowCount()-1, 0));
            }
            playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
        }
        //上一首是历史播放队列的上一首
        else {
            curPlayHistory--;
            while(curPlayHistory >= 0 && !playList_model->haveMedia(playHistory[curPlayHistory])) {
                curPlayHistory--;
            }
            if(curPlayHistory >= 0) {
                playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(0, 0, 0)));
                setCurrentIndex(playList_model->index(playList_model->rowOfPath(playHistory[curPlayHistory]), 0));
                playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
            }
        }
        currentPath = playList_model->media(currentIndex());
        emit preMedia(currentPath);
    }
}

//下一首
void PlayListView::nextOne()
{
    //只有播放列表不为空才有下一首
    if(playList_model->rowCount() != 0) {
        //下一就是播放列表的下一首
        if(currentPlayMode < 3) {
            QModelIndex index = currentIndex();
            playList_model->item(index.row())->setForeground(QBrush(QColor(0, 0, 0)));
            setCurrentIndex(playList_model->index((index.row() + 1) % playList_model->rowCount(), 0));
            playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
        }
        //下一首随机
        else {
            playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(0, 0, 0)));
            setCurrentIndex(playList_model->index(rand() % playList_model->rowCount(), 0));
            playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
        }
        currentPath = playList_model->media(currentIndex());
        emit changeMedia(currentPath);
    }
}

void PlayListView::changePlayMode(int value)
{
    currentPlayMode = PlayMode(value);
}

int PlayListView::playListNum()
{
    return playList_model->rowCount();
}

void PlayListView::normalNextOne()
{
    QModelIndex index = currentIndex();
    switch(currentPlayMode) {
    case PlayMode::CurrentItemOnce:
        break;
    case PlayMode::Sequential:
        playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(0, 0, 0)));
        setCurrentIndex(playList_model->index((index.row() + 1) % playList_model->rowCount(), 0));
        playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
        currentPath = playList_model->media(currentIndex());
        emit changeMedia(currentPath);
        break;
    case PlayMode::Loop:
        emit changeMedia(currentPath);
        break;
    case PlayMode::Random:
        playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(0, 0, 0)));
        setCurrentIndex(playList_model->index(rand() % playList_model->rowCount(), 0));
        playList_model->item(currentIndex().row())->setForeground(QBrush(QColor(255, 0, 0)));
        currentPath = playList_model->media(currentIndex());
        emit changeMedia(currentPath);
        break;
    }
}

int PlayListView::totalMedia()
{
    return playList_model->rowCount();
}

void PlayListView::clearMedia()
{
    QMessageBox::StandardButton result = QMessageBox::question(NULL, "InfinityPlayer", "确定要清空播放列表吗?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes) {
        playList_model->clear();
        emit noMedia();
    }
}

void PlayListView::noExist()
{
    playList_model->removeNoExist(currentIndex());
}

bool PlayListView::existMedia(QString path)
{
    return playList_model->haveMedia(path);
}


