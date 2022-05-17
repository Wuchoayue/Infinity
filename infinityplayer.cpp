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
    setAttribute(Qt::WA_StyledBackground, true);
    //加载qss样式文件
    QFile file(":/qss/infinityPlayer.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    setStyleSheet(stylesheet);
    file.close();

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
    video = new myfullscreen(this);
    duration_slider = video->getPc()->getDuration_slider();
    duration_label = video->getPc()->getDuration_label();
    playerControls = video->getPc();

    //播放列表部分
    playList = new PlayList(this);
    playList->getPlayList_listView()->loadPlayList(infinityPlayer_sqlQuery);
    if(playList->getPlayList_listView()->playListNum() == 0) {
        playerControls->getPreOne_button()->setEnabled(false);
        playerControls->getNextOne_button()->setEnabled(false);
    }

    //布局
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mediaDir, 1);
    layout->addWidget(video, 4);
    layout->addWidget(playList, 1);
    setLayout(layout);

    //后端部分
    player = new Player();
    duration_timer = new QTimer(this);

    //播放初始化
    initPlay();
    duration_timer->setInterval(10);
    connect(duration_timer, &QTimer::timeout, this, [=] {
        currentDuration = currentDuration + currentPlaySpeed;
        duration_slider->setValue(currentDuration);
        //表示播放结束了
        if(duration_slider->value() == duration_slider->maximum()) {
            initPlay();
            nextMedia();
        }
    });

    changeMediaDirShow();

    //信号与槽函数
    //媒体库部分
    //展示目录项表
    connect(mediaDir->getMediaDir_listWidget(), &QListWidget::itemClicked, this, [=] {
        mediaDir->showMediaItem(mediaDir->getMediaDir_listWidget()->currentItem());
    });
    connect(mediaDir, &MediaDir::close_button_clicked, this, &InfinityPlayer::changeMediaDirShow);

    //播放控制部分
    //媒体库开关
    connect(playerControls, &PlayerControls::changeMediaDirShow_signal, this, &InfinityPlayer::changeMediaDirShow);
    //播放模式
    connect(playerControls, &PlayerControls::playMode_signal, this, [&](int value) {
        playList->getPlayList_listView()->changePlayMode(value);
    });
    //上一首
    connect(playerControls, &PlayerControls::preOne_signal, this, [=] {
        playList->getPlayList_listView()->preOne(playHistory, curPlayHistory);
    });
    //播放/暂停
    connect(playerControls, &PlayerControls::playStatus_signal, this, [=] {
        if(!player->Playing()) return;
        if(isPlay) {
            playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus1.png"));
            duration_timer->stop();
        }
        else {
            playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus2.png"));
            duration_timer->start();
        }
        isPlay = !isPlay;
        player->Pause();
    });
    //下一首
    connect(playerControls, &PlayerControls::nextOne_signal, playList->getPlayList_listView(), &PlayListView::nextOne);
    //改变播放速度
    connect(playerControls, &PlayerControls::playSpeed_signal, this, [&](double value) {
        currentPlaySpeed = value;
        player->SetSpeed(value);
        qDebug() << currentPlaySpeed;
    });
    //音量设置
    connect(playerControls, &PlayerControls::volume_signal, this, [&](int value) {
        currentVolume = value;
        player->SetVolume(value);
        if(value == 0) {
            playerControls->getVolume_button()->setIcon(QIcon(":/icon/volume2.png"));
            playerControls->getVolumeShow_button()->setIcon(QIcon(":/icon/volume2.png"));
        }
        else {
            playerControls->getVolume_button()->setIcon(QIcon(":/icon/volume1.png"));
            playerControls->getVolumeShow_button()->setIcon(QIcon(":/icon/volume1.png"));
        }
    });
    //改变屏幕大小
    connect(playerControls, &PlayerControls::fullScreen_signal, this, [=] {
        if(video->isFullScreen()) {
            video->turnToNormal();
            playerControls->getFullScreen_button()->setIcon(QIcon(":/icon/fullScreen1.png"));
        }
        else {
            video->turnToFullScreen();
            playerControls->getFullScreen_button()->setIcon(QIcon(":/icon/fullScreen2.png"));
        }
    });
    //显示列表
    connect(playerControls, &PlayerControls::showList_signal, this, [=] {
        isShowList = !isShowList;
        playList->setVisible(isShowList);
        if(isShowList) playerControls->getShowList_button()->setIcon(QIcon(":/icon/showList_visible.png"));
        else {
            playerControls->getShowList_button()->setIcon(QIcon(":/icon/showList.png"));
        }
    });
    //改变进度
    connect(duration_slider, &DurationSlider::sliderMoved, this, [&](int value) {
        duration_timer->stop();
        player->Jump(value);
        currentDuration = value;
    });
    connect(duration_slider, &DurationSlider::valueChanged, this, [&](int value) {
        QString str = secTotime(value / 100 + 0.5);
        duration_label->setText(str + " / " + mediaDuration_str);
    });
    connect(duration_slider, &DurationSlider::sliderReleasedAt, this, [&](int value) {
        if(isPlay) duration_timer->start();
        player->Jump(value);
        currentDuration = value;
    });
    //改变进度微调大小
    connect(playerControls, &PlayerControls::durationStep_signal, this, [&](int value) {
        currentDurationStep = value;
    });

    //播放列表部分
    //改变播放的音视频
    connect(playList->getPlayList_listView(), &PlayListView::changeMedia, this, &InfinityPlayer::playMedia);
    connect(playList->getPlayList_listView(), &PlayListView::preMedia, this, &InfinityPlayer::on_preMedia);
    //从媒体库添加至播放列表
    connect(mediaDir->getMediaItem_tableView(), &QTableView::doubleClicked, this, [&](const QModelIndex &index) {
        QString path = mediaDir->getMediaItem_sqlQueryModel()->index(index.row(), 1).data().toString();
        playList->getPlayList_listView()->insert(path);
    });
    connect(playList->getPlayList_listView(), &PlayListView::noMedia, this, [=] {
        playerControls->getPreOne_button()->setEnabled(false);
        playerControls->getNextOne_button()->setEnabled(false);
        player->Quit();
        initPlay();
    });
    connect(playList->getPlayList_listView(), &PlayListView::haveMedia, this, [=] {
        playerControls->getPreOne_button()->setEnabled(true);
        playerControls->getNextOne_button()->setEnabled(true);
    });
    //播放全部
    connect(mediaDir, &MediaDir::on_playAll_button_clicked, this, [&](QList<QString> pathes) {
        for(int i=0; i<pathes.size(); i++) {
            if(!playList->getPlayList_listView()->existMedia(pathes[i])) {
                playList->getPlayList_listView()->insert(pathes[i]);
            }
        }
    });
    //添加至播放列表
    connect(mediaDir, &MediaDir::addToPlayList_triggered, this, [&](QString path) {
        playList->getPlayList_listView()->insert(path);
    });
}

InfinityPlayer::~InfinityPlayer()
{
    playList->getPlayList_listView()->savePlayList(infinityPlayer_sqlQuery);
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
        mediaDir->setInfinityPlayer_sqlQuery(infinityPlayer_sqlQuery);
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
                infinityPlayer_sqlQuery->exec("INSERT INTO MediaDir VALUES('新建目录')");
            }
            else {
                qDebug() << creatTableSql;
            }
        }
    }
    if(infinityPlayer_sqlQuery->exec("SELECT * FROM MediaDir")) {
        while(infinityPlayer_sqlQuery->next()) {
            mediaDir->getMediaDir_listWidget()->addItem(infinityPlayer_sqlQuery->value(0).toString());
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
            //没有PlayList表就创建
            QString creatTableSql = QString("CREATE TABLE PlayList("
                                            "path TEXT UNIQUE NOT NULL,"
                                            "cover TEXT NOT NULL,"
                                            "md5 BLOB NOT NULL)");
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
    mediaDir->getMediaDir_listWidget()->setCurrentRow(0);
    if(isMediaDirShow) {
        mediaDir->showMediaItem(mediaDir->getMediaDir_listWidget()->currentItem());
        mediaDir->getMediaItem_label()->setText(mediaDir->getMediaDir_listWidget()->currentItem()->text());
        mediaDir->show();
        video->hide();
        playList->hide();
        isPlay = false;
        player->Pause();
        playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus1.png"));
    }
    else {
        mediaDir->hide();
        video->show();
        if(isShowList) playList->show();
        isPlay = true;
        player->Pause();
        playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus2.png"));
    }
    isMediaDirShow = !isMediaDirShow;
}

void InfinityPlayer::playMedia(QString path)
{
    initPlay();
    video->update();
    video->getVw()->setVisible(true);
    playHistory.append(path);
    curPlayHistory = playHistory.size() - 1;
    isPlay = true;
    duration_slider->setEnabled(true);
    playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus2.png"));
    player->Play(path.toStdString().c_str(), (void*)video->getVw()->winId());
    player->SetSpeed(currentPlaySpeed);
    player->SetVolume(currentVolume);
    mediaDuration = player->GetTotalDuration();
    int cur = mediaDuration + 0.5;
    QString str = secTotime(cur / 100);
    mediaDuration_str = str;
    duration_slider->setMaximum(int(mediaDuration));
    duration_slider->setValue(0);
    duration_timer->start();
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
    video->getVw()->setVisible(false);
    currentDuration = 0;
}

void InfinityPlayer::on_preMedia(QString path)
{
    initPlay();
    video->update();
    video->getVw()->setVisible(true);
    isPlay = true;
    duration_slider->setEnabled(true);
    playerControls->getPlayStatus_button()->setIcon(QIcon(":/icon/playStatus2.png"));
    player->Play(path.toStdString().c_str(), (void*)video->getVw()->winId());
    player->SetVolume(currentVolume);
    player->SetSpeed(currentPlaySpeed);
    mediaDuration = player->GetTotalDuration() * 10;
    int cur = mediaDuration + 0.5;
    QString str = secTotime(cur / 100);
    mediaDuration_str = str;
    duration_slider->setMaximum(int(mediaDuration));
    duration_slider->setValue(0);
    duration_timer->start();
}

void InfinityPlayer::nextMedia()
{
    playList->getPlayList_listView()->normalNextOne();
}

PlayerControls *InfinityPlayer::getPlayerControls() const
{
    return playerControls;
}

void InfinityPlayer::keyPressEvent(QKeyEvent *event)
{
    //上一首
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Left) {
        if(playList->getPlayList_listView()->totalMedia() != 0)
            playList->getPlayList_listView()->preOne(playHistory, curPlayHistory);
    }
    //下一首
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Right) {
        if(playList->getPlayList_listView()->totalMedia() != 0)
            playList->getPlayList_listView()->nextOne();
    }
    //调大音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Up) {
        if(currentVolume < 100)
            playerControls->getVolume_slider()->setValue(currentVolume + 1);
    }
    //调小音量
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Down) {
        if(currentVolume > 0)
            playerControls->getVolume_slider()->setValue(currentVolume - 1);
    }
    //进度微调
    else if(event->key() == Qt::Key_Left) {
        qDebug() << currentDurationStep;
        player->Backward(currentDurationStep);
        currentDuration = player->MyGetCurrentTime();
        duration_slider->setValue(currentDuration);
    }
    else if(event->key() == Qt::Key_Right) {
        qDebug() << currentDurationStep;
        player->Forward(currentDurationStep);
        currentDuration = player->MyGetCurrentTime();
        duration_slider->setValue(currentDuration);
    }
    //改变屏幕大小
    else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
        if(video->isFullScreen()) {
            video->turnToNormal();
            playerControls->getFullScreen_button()->setIcon(QIcon(":/icon/fullScreen1.png"));
        }
        else {
            video->turnToFullScreen();
            playerControls->getFullScreen_button()->setIcon(QIcon(":/icon/fullScreen2.png"));
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
                QMessageBox::critical(NULL, "InfinityPlayer", "不支持该格式文件", QMessageBox::Ok);
                return;
            }
        }
        for(int i=0; i<pathes.size(); i++) {
            playList->getPlayList_listView()->insert(pathes[i].toLocalFile());
        }
    }
}

