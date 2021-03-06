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
    this->resize(800, 480);
    setMinimumWidth(800);
    setMinimumHeight(480);
    setAcceptDrops(true); //设置可以接收拖动事件
    grabKeyboard();
    setMouseTracking(true);

    //添加播放器支持类型
    supportType.insert(".mp3");
    supportType.insert(".mp4");
    supportType.insert(".wav");
    supportType.insert(".avi");
    supportType.insert(".flv");

    //媒体库部分
    mediaDir = new MediaDir(this);

    //读入目录
    loadMediaDir();

    //播放控制部分
    video_videoWidget = new MyVideoWidget(this);
    playerControls = new PlayerControls(this);
    duration_label = new QLabel(this);
    duration_label->setText("00:00 / 00:00");
    duration_slider = new DurationSlider(this);
    duration_slider->setOrientation(Qt::Horizontal);
    duration_slider->setMinimum(0);

    //播放列表部分
    playList = new PlayList(this);
    playList->playList_listView->loadPlayList(infinityPlayer_sqlQuery);
    if(playList->playList_listView->playListNum() == 0) {
        playerControls->preOne_button->setEnabled(false);
        playerControls->nextOne_button->setEnabled(false);
    }
    QHBoxLayout *layout_center = new QHBoxLayout;
    layout_center->setContentsMargins(0, 0, 0, 0);
    layout_center->addWidget(mediaDir, 1);
    layout_center->addWidget(video_videoWidget, 4);
    layout_center->addWidget(playList, 1);
    QHBoxLayout *layout_bottom = new QHBoxLayout;
    layout_bottom->addWidget(duration_slider);
    layout_bottom->addWidget(duration_label);
    setLayout(layout_center);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(layout_bottom);
    layout->addWidget(playerControls);
    video_videoWidget->playControls_widget->setLayout(layout);
    video_videoWidget->init();

    //后端部分
    player = new Player();
    duration_timer = new QTimer(this);
    playerControls_timer = new QTimer(this);
    playerControls_timer->setInterval(500);
    //播放初始化
    initPlay();
    connect(duration_timer, &QTimer::timeout, this, [=] {
        int cur = duration_slider->value() + currentPlaySpeed;
        duration_slider->setValue(cur);
        //表示播放结束了
        if(!player->Playing()) {
            initPlay();
            nextMedia();
            qDebug() << 1;
        }
    });
    connect(playerControls_timer, &QTimer::timeout, this, [=] {
        QPoint now = QCursor::pos();
        if(now != prePoint) {
            video_videoWidget->playControls_widget->show();
            video_videoWidget->playControls_widget->raise();
            prePoint = now;
            noChange = 0;
        }
        else {
            noChange += 1;
            if(noChange >= 10) {
                video_videoWidget->playControls_widget->hide();
            }
        }
    });
    playerControls_timer->start();
    changeMediaDirShow();

    //信号与槽函数
    //媒体库部分
    //展示目录项表
    connect(mediaDir->mediaDir_listWidget, &QListWidget::itemClicked, this, [=] {
        mediaDir->showMediaItem(mediaDir->mediaDir_listWidget->currentItem());
        if(isPlay) {
            player->Pause();
            isPlay = false;
            playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    });
    //添加目录
    connect(mediaDir, &MediaDir::dirName_lineEdit_editingFinished, this, [=] {
        mediaDir->addMediaDir(infinityPlayer_sqlQuery);
    });
    //删除目录
    connect(mediaDir, &MediaDir::delMediaDir_button_clicked, this, [=] {
        mediaDir->delMediaDir(infinityPlayer_sqlQuery);
    });
    //重命名
    connect(mediaDir, &MediaDir::dirName_lineEdit_1_editingFinished, this, [=] {
        mediaDir->renameMediaDir(infinityPlayer_sqlQuery);
    });
    //添加目录项
    connect(mediaDir, &MediaDir::addMediaItem_button_clicked, this, [=] {
        mediaDir->addMediaItem(infinityPlayer_sqlQuery);
    });
    //删除目录项
    connect(mediaDir, &MediaDir::delMediaItem_button_clicked, this, [=] {
        mediaDir->delMediaItem(infinityPlayer_sqlQuery);
    });

    //播放控制部分
    //媒体库开关
    connect(playerControls, &PlayerControls::changeMediaDirShow_signal, this, &InfinityPlayer::changeMediaDirShow);
    //播放模式
    connect(playerControls, &PlayerControls::playMode_signal, this, [&](int value) {
        playList->playList_listView->changePlayMode(value);
    });
    //上一首
    connect(playerControls, &PlayerControls::preOne_signal, this, [=] {
        playList->playList_listView->preOne(playHistory, curPlayHistory);
    });
    //播放/暂停
    connect(playerControls, &PlayerControls::playStatus_signal, this, [=] {
        if(isPlay) {
            playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            duration_timer->stop();
        }
        else {
            playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            duration_timer->start(1000);
        }
        isPlay = !isPlay;
        player->Pause();
    });
    //下一首
    connect(playerControls, &PlayerControls::nextOne_signal, playList->playList_listView, &PlayListView::nextOne);
    //改变播放速度
    connect(playerControls, &PlayerControls::playSpeed_signal, this, [&](int value) {
        currentPlaySpeed = value;
        player->SetSpeed(value);
    });
    //音量设置
    connect(playerControls, &PlayerControls::volume_signal, this, [&](int value) {
        currentVolume = value;
        player->SetVolume(value);
    });
    //改变屏幕大小
    connect(playerControls, &PlayerControls::fullScreen_signal, this, [=] {
        if(!video_videoWidget->isFullScreen()) {
            if(first) {
                video_videoWidget->setFullScreen(true);
                video_videoWidget->setFullScreen(false);
                video_videoWidget->setFullScreen(true);
                video_videoWidget->playControls_widget->raise();
                if(isPlay) first = false;
            }
            else {
                video_videoWidget->setFullScreen(true);
                video_videoWidget->playControls_widget->raise();
            }
        }
        else {
            video_videoWidget->setFullScreen(false);
        }
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
        if(isPlay) duration_timer->start(1000);
        player->Jump(value);
    });

    //播放列表部分
    //改变播放的音视频
    connect(playList->playList_listView, &PlayListView::changeMedia, this, &InfinityPlayer::playMedia);
    connect(playList->playList_listView, &PlayListView::preMedia,this, &InfinityPlayer::on_preMedia);
    //从媒体库添加至播放列表
    connect(mediaDir->mediaItem_tableView, &QTableView::doubleClicked, this, [&](const QModelIndex &index) {
        QSqlQueryModel mediaItem_sqlQueryModel;
        QString path = mediaItem_sqlQueryModel.index(index.row(), 1).data().toString();
        playList->playList_listView->insert(path);
    });
    connect(playList->playList_listView, &PlayListView::noMedia, this, [=] {
        playerControls->preOne_button->setEnabled(false);
        playerControls->nextOne_button->setEnabled(false);
    });
    connect(playList->playList_listView, &PlayListView::haveMedia, this, [=] {
        playerControls->preOne_button->setEnabled(true);
        playerControls->nextOne_button->setEnabled(true);
    });
}

InfinityPlayer::~InfinityPlayer()
{
    playList->playList_listView->savePlayList(infinityPlayer_sqlQuery);
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
            mediaDir->mediaDir_listWidget->addItem(infinityPlayer_sqlQuery->value(0).toString());
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

void InfinityPlayer::changeMediaDirShow()
{
    mediaDir->mediaDir_listWidget->setCurrentRow(0);
    mediaDir->mediaItem_label->setText(mediaDir->mediaDir_listWidget->currentItem()->text());
    mediaDir->setVisible(isMediaDirShow);
    video_videoWidget->setVisible(!isMediaDirShow);
    playList->setVisible(!isMediaDirShow);
    if(!isMediaDirShow) {
        video_videoWidget->playControls_widget->show();
        playerControls_timer->start();
    }
    else {
        video_videoWidget->playControls_widget->hide();
        playerControls_timer->stop();
    }
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
        playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
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
        playerControls->playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
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
    playList->playList_listView->normalNextOne();
}

void InfinityPlayer::keyPressEvent(QKeyEvent *event)
{
    //上一首
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left) {
        if(playList->playList_listView->totalMedia() != 0)
            playList->playList_listView->preOne(playHistory, curPlayHistory);
    }
    //下一首
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right) {
        if(playList->playList_listView->totalMedia() != 0)
            playList->playList_listView->nextOne();
    }
    //调大音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Up) {
        if(currentVolume < 100)
            playerControls->volume_slider->setValue(currentVolume + 1);
    }
    //调小音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Down) {
        if(currentVolume > 0)
            playerControls->volume_slider->setValue(currentVolume - 1);
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
            video_videoWidget->setFullScreen(false);
        }
        else {
            if(first) {
                video_videoWidget->setFullScreen(true);
                video_videoWidget->setFullScreen(false);
                video_videoWidget->setFullScreen(true);
                video_videoWidget->playControls_widget->raise();
                first = false;
            }
            else {
                video_videoWidget->setFullScreen(true);
                video_videoWidget->playControls_widget->raise();
            }
        }
    }
    //播放/暂停
    else if(event->key() == Qt::Key_Space) {
        emit playerControls->playStatus_signal();
    }
    //关闭媒体目录
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Q) {
        changeMediaDirShow();
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
            playList->playList_listView->insert(pathes[i].toLocalFile());
        }
    }
}

void InfinityPlayer::moveEvent(QMoveEvent *event)
{
    QPoint cur = mapToGlobal(QPoint(0, 0));
    video_videoWidget->playControls_widget->move(cur.x(), cur.y() + video_videoWidget->height() * 0.8);
    return QWidget::moveEvent(event);
}



