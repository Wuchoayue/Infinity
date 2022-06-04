#include "infinityplayer.h"
#include "./ui_infinityplayer.h"
#include <QAudioDevice>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QListWidget>


InfinityPlayer::InfinityPlayer(QWidget *parent)
    : QWidget(parent)
{
    // 初始化
    this->resize(680, 480);
    setAcceptDrops(true); //设置可以接收拖动事件
    grabKeyboard();
    setStyleSheet(video_widget_style);
//    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    //    setAttribute(Qt::WA_TranslucentBackground);

    //添加播放器支持类型
    supportType.insert(".mp3");
    supportType.insert(".mp4");
    supportType.insert(".wav");
    supportType.insert(".avi");
    supportType.insert(".flv");
    //媒体库部分
    mediaDir_listWidget = new QListWidget(this);
    mediaDir_listWidget->setGridSize(QSize(mediaDir_listWidget->width(), mediaDir_listWidget->height() * 0.8));
    mediaDir_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    dirName_lineEdit = new QLineEdit(mediaDir_listWidget);
    dirName_lineEdit->resize(mediaDir_listWidget->gridSize());
    dirName_lineEdit->setVisible(false);
    dirName_lineEdit_1 = new QLineEdit(mediaDir_listWidget);
    dirName_lineEdit_1->resize(mediaDir_listWidget->gridSize());
    dirName_lineEdit_1->setVisible(false);
    mediaItem_tableView = new QTableView(this);
    mediaDir_label = new QLabel("音视频目录", this);
    addMediaDir_button = new QToolButton(this);
    addMediaDir_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));
    delMediaDir_button = new QToolButton(this);
    delMediaDir_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
    mediaItem_tableView->setVisible(false);
    mediaItem_label = new QLabel("粤语歌曲", this);
    addMediaItem_button = new QToolButton(this);
    addMediaItem_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));
    delMediaItem_button = new QToolButton(this);
    delMediaItem_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));


    //读入目录
    loadMediaDir();

    //播放控制部分
    video_videoWidget = new myfullscreen(this);
    playerControls = video_videoWidget->pc;
//    PlayerControls* pc = new PlayerControls(this);
//    pc->raise();
//    pc->setGeometry(0,height()-40,width(),40);
//    pc->show();

    duration_label = new QLabel(this);
    duration_label->setText("00:00 / 00:00");
    duration_slider = new DurationSlider(this);
    duration_slider->setOrientation(Qt::Horizontal);
    duration_slider->setMinimum(0);
    playerControls->duration_label = duration_label;
    playerControls->duration_slider = duration_slider;

    //播放列表部分
    playList_listView = new PlayListView(this);
    playList_listView->loadPlayList(infinityPlayer_sqlQuery);
    if(playList_listView->playListNum() == 0) {
        playerControls->preOne_button->setEnabled(false);
        playerControls->nextOne_button->setEnabled(false);
    }



    //layout部分
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *layout_bottom = new QHBoxLayout;
    QHBoxLayout *layout_topleft = new QHBoxLayout;
    QVBoxLayout *layout_left = new QVBoxLayout;
    QHBoxLayout *layout_tabletop = new QHBoxLayout;
    QVBoxLayout *layout_table = new QVBoxLayout;
    QHBoxLayout *layout_center = new QHBoxLayout;


    layout_topleft->setContentsMargins(0, 0, 0, 0);
    layout_topleft->addWidget(mediaDir_label,2);
    layout_topleft->addWidget(addMediaDir_button, 1);
    layout_topleft->addWidget(delMediaDir_button, 1);

    layout_left->setContentsMargins(0, 0, 0, 0);
    layout_left->addLayout(layout_topleft, 1);
    layout_left->addWidget(mediaDir_listWidget, 6);

    layout_tabletop->setContentsMargins(0, 0, 0, 0);
    layout_tabletop->addWidget(mediaItem_label, 1);
    layout_tabletop->addWidget(addMediaItem_button, 1);
    layout_tabletop->addWidget(delMediaItem_button, 1);

    layout_topleft->setSpacing(0);
    layout_table->setContentsMargins(0, 0, 0, 0);
    layout_table->addLayout(layout_tabletop, 1);
    layout_table->addWidget(mediaItem_tableView, 6);

    layout_center->setContentsMargins(0, 0, 0, 0);
    layout_center->addLayout(layout_left, 1);
    layout_center->addLayout(layout_table, 4);
    layout_center->addWidget(video_videoWidget, 4);
    layout_center->addWidget(playList_listView, 1);

//    layout_bottom->addWidget(duration_slider);
//    layout_bottom->addWidget(duration_label);
    playerControls->progressBar_layout->addWidget(duration_slider);
    playerControls->progressBar_layout->addWidget(duration_label);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(layout_center, 9);
//    layout->addLayout(layout_bottom, 1);
//    layout->addWidget(fpc, 1);



    setLayout(layout);
    changeMediaDirShow();
//    video_videoWidget->init();
    //后端部分
    player = new Player();
    duration_timer = new QTimer(this);
    //播放初始化
    initPlay();
    connect(duration_timer, &QTimer::timeout, this, [=] {
        int cur = duration_slider->value() + currentPlaySpeed;
        duration_slider->setValue(cur);
        //表示播放结束了
        if(!player->Playing()) {
            initPlay();
            nextMedia();
        }
    });
    //全屏窗口初始化
//    video_videoWidget->init();

    //信号与槽函数
    //媒体库部分
    //展示目录项表
    connect(mediaDir_listWidget, &QListWidget::itemClicked, this, &InfinityPlayer::showMediaItem);
    //添加目录
    connect(addMediaDir_button, &QToolButton::clicked, this, &InfinityPlayer::on_addMediaDir_button_clicked);
    connect(dirName_lineEdit, &QLineEdit::editingFinished, this, &InfinityPlayer::addMediaDir);
    //删除目录
    connect(delMediaDir_button, &QToolButton::clicked, this, &InfinityPlayer::on_delMediaDir_button_clicked);
    //重命名
    connect(mediaDir_listWidget, &QListWidget::customContextMenuRequested, this, &InfinityPlayer::on_mediaDir_menu);
    connect(dirName_lineEdit_1, &QLineEdit::editingFinished, this, &InfinityPlayer::changeMediaDir);
    //添加目录项
    connect(addMediaItem_button, &QToolButton::clicked, this, &InfinityPlayer::on_addMediaItem_button_clicked);
    //删除目录项
    connect(delMediaItem_button, &QToolButton::clicked, this, &InfinityPlayer::on_delMediaItem_button_clicked);

    //播放控制部分
    //媒体库开关
    connect(playerControls, &PlayerControls::changeMediaDirShow_signal, this, &InfinityPlayer::changeMediaDirShow);
    //播放模式
    connect(playerControls, &PlayerControls::playMode_signal, this, [&](int value) {
        playList_listView->changePlayMode(value);
    });
    //上一首
    connect(playerControls, &PlayerControls::preOne_signal, this, [=] {
        playList_listView->preOne(playHistory, curPlayHistory);
    });
    //播放/暂停
    connect(playerControls, &PlayerControls::playStatus_signal, this, [=] {
        if(isPlay) {
            playerControls->playStatus_button->setStyleSheet(playStatus_button_style);
            duration_timer->stop();
        }
        else {
            playerControls->playStatus_button->setStyleSheet(playStatus_button_style_2);
            duration_timer->start(1000);
        }
        isPlay = !isPlay;
        player->Pause();
    });
    //下一首
    connect(playerControls, &PlayerControls::nextOne_signal, playList_listView, &PlayListView::nextOne);
    //改变播放速度
    connect(playerControls, &PlayerControls::playSpeed_signal, this, [&](int value) {
        currentPlaySpeed = value;
        player->SetSpeed(value);
    });
    //音量设置
    connect(playerControls, &PlayerControls::volume_signal, this, [&](int value) {
        currentVolume = value;
        player->SetVolume(value);
        if(value==0){
            playerControls->volume_button->setStyleSheet(volume_button_style_2);
        }
        else{
            playerControls->volume_button->setStyleSheet(volume_button_style);
        }
    });
    //改变屏幕大小
    connect(playerControls, &PlayerControls::fullScreen_signal, this, [&](bool value) {
        if(!video_videoWidget->isFullScreen()){
        video_videoWidget->turnToFullScreen();
        playerControls->fullScreen_button->setStyleSheet(fullScreen_button_style_2);
        }
        else{
            video_videoWidget->turnToNormal();
            playerControls->fullScreen_button->setStyleSheet(fullScreen_button_style);
        }
    });
    //显示列表
    connect(playerControls, &PlayerControls::showList_signal, this, [&](bool value) {
            playList_listView->setVisible(!value);
            playerControls->setShowListVisable(!value);
    });
    //改变进度
    connect(duration_slider, &DurationSlider::sliderMoved, this, [&](int value) {
        duration_timer->stop();
        player->Jump(value);
    });
    connect(duration_slider, &DurationSlider::valueChanged, this, [&](int value) {
        QString str = secTotime(value);
        duration_label->setText(str + " / " + mediaDuration_str);
    });
    connect(duration_slider, &DurationSlider::sliderReleasedAt, this, [&](int value) {
        duration_timer->start(1000);
        player->Jump(value);
    });

    //播放列表部分
    //改变播放的音视频
    connect(playList_listView, &PlayListView::changeMedia, this, &InfinityPlayer::playMedia);
    connect(playList_listView, &PlayListView::preMedia,this, &InfinityPlayer::on_preMedia);
    //从媒体库添加至播放列表
    connect(mediaItem_tableView, &QTableView::doubleClicked, this, [&](const QModelIndex &index) {
        QString path = mediaItem_sqlQueryModel.index(index.row(), 1).data().toString();
        playList_listView->insert(path);
    });
    connect(playList_listView, &PlayListView::noMedia, this, [=] {
        playerControls->preOne_button->setEnabled(false);
        playerControls->nextOne_button->setEnabled(false);
    });
    connect(playList_listView, &PlayListView::haveMedia, this, [=] {
        playerControls->preOne_button->setEnabled(true);
        playerControls->nextOne_button->setEnabled(true);
    });


}

InfinityPlayer::~InfinityPlayer()
{
    playList_listView->savePlayList(infinityPlayer_sqlQuery);
}

//加载媒体库目录
void InfinityPlayer::loadMediaDir()
{
    //添加驱动
    infinityPlayer_dataBase = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名字a
    infinityPlayer_dataBase.setDatabaseName("InfinityPlayer.db");
    //打开数据库
    if(infinityPlayer_dataBase.open()) {
        qDebug() << "Success";
        infinityPlayer_sqlQuery = new QSqlQuery(infinityPlayer_dataBase);
    }
    else {
        qDebug() << "Fail";
    }
    //判断是否存在数据表--MediaDir
    QString sql = QString("select * from sqlite_master where name='%1'").arg("MediaDir");
    if(infinityPlayer_sqlQuery->exec(sql)) {
        if(infinityPlayer_sqlQuery->next()) qDebug() << "Exist";
        else {
            //没有MeidaDir表就创建
            QString creatTableSql = QString("CREATE TABLE MediaDir("
                                            "dirname TEXT UNIQUE NOT NULL)");
            if(infinityPlayer_sqlQuery->exec(creatTableSql)) {
                qDebug() << "Create Table -- MediaDir Success";
            }
            else {
                qDebug() << creatTableSql;
            }
        }
    }
    if(infinityPlayer_sqlQuery->exec("SELECT * FROM MediaDir")) {
        while(infinityPlayer_sqlQuery->next()) {
            mediaDir_listWidget->addItem(infinityPlayer_sqlQuery->value(0).toString());
        }
    }
    else {
        qDebug() << "Fail to lada MediaDir";
    }
    //判断是否存在数据表--MediaItem
    sql = QString("select * from sqlite_master where name='%1'").arg("MediaItem");
    if(infinityPlayer_sqlQuery->exec(sql)) {
        if(infinityPlayer_sqlQuery->next()) qDebug() << "Exist";
        else {
            //没有MeidaDir表就创建
            QString creatTableSql = QString("CREATE TABLE MediaItem("
                                            "dirname TEXT NOT NULL,"
                                            "name TEXT NOT NULL,"
                                            "path TEXT NOT NULL,"
                                            "type TEXT NOT NULL)");
            if(infinityPlayer_sqlQuery->exec(creatTableSql)) {
                qDebug() << "Create Table -- MediaItem Success";
            }
            else {
                qDebug() << creatTableSql;
            }
        }
    }
    //判断是否存在数据表--PlayList
    sql = QString("select * from sqlite_master where name='%1'").arg("PlayList");
    if(infinityPlayer_sqlQuery->exec(sql)) {
        if(infinityPlayer_sqlQuery->next()) qDebug() << "Exist";
        else {
            //没有MeidaDir表就创建
            QString creatTableSql = QString("CREATE TABLE PlayList("
                                            "path TEXT UNIQUE NOT NULL,"
                                            "cover TEXT NOT NULL)");
            if(infinityPlayer_sqlQuery->exec(creatTableSql)) {
                qDebug() << "Create Table -- PlayList Success";
            }
            else {
                qDebug() << creatTableSql;
            }
        }
    }
}

void InfinityPlayer::on_addMediaDir_button_clicked()
{

    dirName_lineEdit->move(0, mediaDir_listWidget->count() * dirName_lineEdit->size().height());
    dirName_lineEdit->setText("新建目录");
    dirName_lineEdit->setVisible(true);
}

void InfinityPlayer::addMediaDir()
{
    QString dirname = dirName_lineEdit->text();
    dirName_lineEdit->setVisible(false);
    if(infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
        mediaDir_listWidget->addItem(dirname);
    }
    else {
        dirname += "_0";
        while(!infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
            dirname += "_0";
        }
        mediaDir_listWidget->addItem(dirname);
        mediaDir_listWidget->setCurrentRow(mediaDir_listWidget->count() - 1);
        showMediaItem(mediaDir_listWidget->currentItem());
    }
}

void InfinityPlayer::on_delMediaDir_button_clicked()
{
    if(mediaDir_listWidget->count() == 1) {
        qDebug() << "至少有一个目录!";
        return;
    }
    QListWidgetItem *item = mediaDir_listWidget->currentItem();
    QString dirname = item->data(0).toString();
    QString sql = QString("delete from MediaItem where dirname = '%1'").arg(dirname);
    if(infinityPlayer_sqlQuery->exec(sql)) {
        sql = QString("delete from MediaDir where dirname = '%1'").arg(dirname);
        if(infinityPlayer_sqlQuery->exec(sql)) {
            mediaDir_listWidget->takeItem(mediaDir_listWidget->currentRow());
            mediaDir_listWidget->setCurrentRow(0);
            showMediaItem(mediaDir_listWidget->currentItem());
        }
        else {
            qDebug() << sql;
        }
    }
    else {
        qDebug() << sql;
    }
}

void InfinityPlayer::changeMediaDir()
{
    QListWidgetItem *item = mediaDir_listWidget->currentItem();
    QString oldname = item->data(0).toString();
    QString newname = dirName_lineEdit_1->text();
    if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaDir SET dirname = '%1' WHERE dirname = '%2'").arg(newname, oldname))) {
        if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaItem SET dirname = '%1' WHERE dirname = '%2'").arg(newname, oldname))) {
            item->setData(0, newname);
            dirName_lineEdit_1->setVisible(false);
            mediaItem_label->setText(newname);
        }
        else {
            qDebug() << "修改失败!";
        }
    }
    else {
        qDebug() << "修改失败!";
    }
}

void InfinityPlayer::showMediaItem(QListWidgetItem *item)
{
    QString dirname = item->data(0).toString();
    mediaItem_label->setText(dirname);
    if(isPlay) {
        player->Pause();
        isPlay = false;
        playerControls->playStatus_button->setStyleSheet(playStatus_button_style);
    }
    playerControls->playStatus_button->setStyleSheet(playStatus_button_style);
    QString str = QString("SELECT name as Name, path as Path, type as Type FROM MediaItem WHERE dirname = '%1'").arg(dirname);
    mediaItem_sqlQueryModel.setQuery(str);
    mediaItem_tableView->setModel(&mediaItem_sqlQueryModel);
}

void InfinityPlayer::on_addMediaItem_button_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"open file",".","Audio (*.mp3 *.wav);;Video (*.mp4 *.avi *.flv)");
    if(path != "") {
        int a = path.lastIndexOf("/");
        int b = path.lastIndexOf(".");
        QListWidgetItem *item = mediaDir_listWidget->currentItem();
        QString dirname = item->data(0).toString();
        QString name = path.mid(a + 1, b - a - 1);
        QString type = path.right(path.length() - b - 1);
        QString sql0 = QString("SELECT * FROM MediaItem WHERE dirname = '%1' and path = '%2'").arg(dirname, path);
        infinityPlayer_sqlQuery->exec(sql0);
        if(!infinityPlayer_sqlQuery->next()) {
            QString sql = QString("INSERT INTO MediaItem VALUES('%1', '%2', '%3', '%4')").arg(dirname, name, path, type);
            if(infinityPlayer_sqlQuery->exec(sql)) {
                showMediaItem(item);
            }
            else {
                qDebug() << "插入失败!";
            }
        }
        else {
            qDebug() << "插入失败!";
        }
    }
}

void InfinityPlayer::on_delMediaItem_button_clicked()
{
    QModelIndex index = mediaItem_tableView->currentIndex();
    QString path = mediaItem_sqlQueryModel.index(index.row(), 1).data().toString();
    QString dirname = mediaItem_label->text();
    QString sql = QString("DELETE FROM MediaItem WHERE dirname = '%1' and path = '%2'").arg(dirname, path);
    if(infinityPlayer_sqlQuery->exec(sql)) {
        showMediaItem(mediaDir_listWidget->currentItem());
    }
    else {
        qDebug() << "删除失败!";
    }
}

void InfinityPlayer::on_mediaDir_menu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    QAction* change = new QAction(tr("重命名"), menu);
    connect(change, &QAction::triggered, this, [=] {
        QListWidgetItem *item = mediaDir_listWidget->itemAt(pos);
        QModelIndex index = mediaDir_listWidget->indexAt(pos);
        QString oldname = item->data(0).toString();
        dirName_lineEdit_1->setText(oldname);
        dirName_lineEdit_1->move(0, index.row() * dirName_lineEdit_1->size().height());
        dirName_lineEdit_1->setVisible(true);
    });
    if (mediaDir_listWidget->itemAt(pos)!=nullptr)
    {
        menu->addAction(change);
        menu->popup(mediaDir_listWidget->mapToGlobal(pos));
    }
}

void InfinityPlayer::changeMediaDirShow()
{
    mediaDir_listWidget->setCurrentRow(0);
//               qDebug()<<-1;
//    mediaItem_label->setText(mediaDir_listWidget->currentItem()->text());
//                   qDebug()<<-3;
    mediaItem_tableView->setVisible(isMediaDirShow);
    mediaItem_label->setVisible(isMediaDirShow);
    addMediaItem_button->setVisible(isMediaDirShow);
    delMediaItem_button->setVisible(isMediaDirShow);
    mediaDir_label->setVisible(isMediaDirShow);
    mediaDir_listWidget->setVisible(isMediaDirShow);
    addMediaDir_button->setVisible(isMediaDirShow);
    delMediaDir_button->setVisible(isMediaDirShow);
    video_videoWidget->setVisible(!isMediaDirShow);
    playList_listView->setVisible(!isMediaDirShow);
    isMediaDirShow = !isMediaDirShow;
}

void InfinityPlayer::playMedia(QString path)
{
    initPlay();
    if(path != currentPath) {
        playHistory.append(path);
        curPlayHistory = playHistory.size() - 1;
        currentPath = path;
        isPlay = true;
        duration_slider->setEnabled(true);
        player->SetSpeed(currentPlaySpeed);
        playerControls->playStatus_button->setStyleSheet(playStatus_button_style);
        player->SetVolume(currentVolume);
        player->Play(path.toStdString().c_str(), (void*)video_videoWidget->vw->winId());
        mediaDuration = player->GetTotalDuration();
        int cur = mediaDuration + 0.5;
        QString str = secTotime(cur);
        mediaDuration_str = str;
        duration_slider->setMaximum(int(mediaDuration));
        duration_slider->setValue(0);
        duration_timer->start(1000);
    }
}


QString InfinityPlayer::secTotime(int second)
{
    int hour, min, sec;
    sec = second % 60;
    min = second / 60 % 60;
    hour = second / 3600;
    QString str = QString("%1:%2").arg(min, 2, 10, QLatin1Char('0')).arg(sec, 2, 10, QLatin1Char('0'));
    str = (hour == 0 ? "" : QString::number(hour) + ":") + str;
    return str;
}

void InfinityPlayer::initPlay()
{

    duration_slider->setValue(0);
    duration_slider->setMaximum(100);
    duration_slider->setEnabled(false);
    isPlay = false;
    duration_timer->stop();
    duration_label->setText("00:00 / 00:00");
    currentPath = "";
}

void InfinityPlayer::on_preMedia(QString path)
{
    initPlay();
    if(path != currentPath) {
        currentPath = path;
        isPlay = true;
        duration_slider->setEnabled(true);
        player->SetSpeed(currentPlaySpeed);
        playerControls->playStatus_button->setStyleSheet(playStatus_button_style_2);
        player->SetVolume(currentVolume);
        player->Play(path.toStdString().c_str(), (void*)video_videoWidget->winId());
        mediaDuration = player->GetTotalDuration();
        int cur = mediaDuration + 0.5;
        QString str = secTotime(cur);
        mediaDuration_str = str;
        duration_slider->setMaximum(int(mediaDuration));
        duration_slider->setValue(0);
        duration_timer->start(1000);
    }
}

void InfinityPlayer::nextMedia()
{
    playList_listView->normalNextOne();
}

void InfinityPlayer::keyPressEvent(QKeyEvent *event)
{
    //上一首
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left) {
        if(playList_listView->totalMedia() != 0)
            playList_listView->preOne(playHistory, curPlayHistory);
    }
    //下一首
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right) {
        if(playList_listView->totalMedia() != 0)
            playList_listView->nextOne();
    }
    //调大音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Up) {
            playerControls->volume_button->setStyleSheet(volume_button_style);
        if(currentVolume < 100)
            playerControls->volume_slider->setValue(currentVolume + 1);
    }
    //调小音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Down) {
        if(currentVolume > 0)
            playerControls->volume_slider->setValue(currentVolume - 1);
        else{
            playerControls->volume_button->setStyleSheet(volume_button_style_2);
        }
    }
    //进度微调
    else if(event->key() == Qt::Key_Left) {
        player->Backward();
        duration_slider->setValue(duration_slider->value() - 8 > 0 ? duration_slider->value() - 8 : 0);
    }
    else if(event->key() == Qt::Key_Right) {
        player->Forward();
        duration_slider->setValue(duration_slider->value() + 8 < duration_slider->maximum() ? duration_slider->value() + 8 : duration_slider->maximum());
    }
    //改变屏幕大小
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
        if(video_videoWidget->isFullScreen()) {
            video_videoWidget->turnToNormal();
            playerControls->fullScreen_button->setStyleSheet(fullScreen_button_style);
        }
        else{
            video_videoWidget->turnToFullScreen();
            playerControls->fullScreen_button->setStyleSheet(fullScreen_button_style_2);
        }
    }
    //播放/暂停
    else if(event->key() == Qt::Key_Space) {
        emit playerControls->playStatus_signal();
    }
}

void InfinityPlayer::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void InfinityPlayer::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls()) {
        QList<QUrl> pathes = mimeData->urls();
        for(int i=0; i<pathes.size(); i++) {
            QString path = pathes[i].fileName();
            if(!supportType.contains(path.right(path.length() - path.indexOf(".")))) {
                return;
            }
        }
        for(int i=0; i<pathes.size(); i++) {
            playList_listView->insert(pathes[i].toLocalFile());
        }
    }
}



