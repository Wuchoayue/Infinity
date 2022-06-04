#include "playercontrols.h"
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    //加载qss样式文件
    QFile file(":/qss/playerControls.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    setStyleSheet(stylesheet);
    file.close();

    //进度条
    duration_label = new QLabel(this);
    duration_label->setText("00:00 / 00:00");
    duration_slider = new DurationSlider(this);
    duration_slider->setOrientation(Qt::Horizontal);
    duration_slider->setMinimum(0);

    //改变目录显示状态
    changeMediaDirShow_button = new QToolButton(this);
    changeMediaDirShow_button->setObjectName("changeMediaDirShow");
    changeMediaDirShow_button->setIcon(QIcon(":/icon/changeMediaDirShow.png"));
    connect(changeMediaDirShow_button, &QToolButton::clicked, this, [=] {
        emit changeMediaDirShow_signal();
    });

    //进度变化幅度
    durationStep_button = new QPushButton("+4", this);
    durationStep_button->setObjectName("durationStep");
    durationStep_button->setFixedSize(21, 21);
    durationStep_button->setFlat(true);
    connect(durationStep_button, &QPushButton::clicked, this, [=] {
        if(durationStep_value == 4) {
            durationStep_value = 6;
            durationStep_button->setText("+6");
        }
        else if(durationStep_value == 6) {
            durationStep_value = 8;
            durationStep_button->setText("+8");
        }
        else if(durationStep_value == 8) {
            durationStep_value = 4;
            durationStep_button->setText("+4");
        }
        emit durationStep_signal(durationStep_value);
    });

    //播放模式控制
    playMode_button = new QToolButton(this);
    playMode_button->setObjectName("playMode");
    playMode_button->setIcon(QIcon(":/icon/playMode3.png"));
    connect(playMode_button, &QToolButton::clicked, this, [=] {
        QPoint pos;
        pos.setX(0);
        QMenu *playMode_menu = new QMenu(this);
        QAction *playMode_action[4];
        QString playMode_icon[4] = {":/icon/playMode4.png", ":/icon/playMode3.png", ":/icon/playMode2.png", ":/icon/playMode1.png"};
        QString playMode_str[4] = {"只播放当前", "顺序播放", "循环播放", "随机播放"};
        for(int i=3; i>=0; i--) {
            playMode_action[i] = playMode_menu->addAction(QIcon(playMode_icon[i]), playMode_str[i]);
            connect(playMode_action[i], &QAction::triggered, this, [=] {
                if(currentPlayMode != i) {
                    currentPlayMode = i;
                    playMode_button->setIcon(QIcon(playMode_icon[i]));
                    emit playMode_signal(i);
                }
            });
            playMode_action[i]->setCheckable(false);
        }
        pos.setY(-playMode_menu->sizeHint().height());
        playMode_menu->exec(playMode_button->mapToGlobal(pos));
    });

    //上一首
    preOne_button = new QToolButton(this);
    preOne_button->setObjectName("preOne");
    preOne_button->setIcon(QIcon(":/icon/preOne.png"));
    connect(preOne_button, &QToolButton::clicked, this, [=] {
        emit preOne_signal();
    });

    //播放状态
    playStatus_button = new QToolButton(this);
    playStatus_button->setObjectName("playStatus");
    playStatus_button->setIcon(QIcon(":/icon/playStatus1.png"));
    connect(playStatus_button, &QToolButton::clicked, this, [=] {
        emit playStatus_signal();
    });

    //下一首
    nextOne_button = new QToolButton(this);
    nextOne_button->setObjectName("nextOne");
    nextOne_button->setIcon(QIcon(":/icon/nextOne.png"));
    connect(nextOne_button, &QToolButton::clicked, this, [=] {
        emit nextOne_signal();
    });

    //播放速度控制
    playSpeed_button = new QPushButton("倍速", this);
    playSpeed_button->setObjectName("playSpeed");
    playSpeed_button->setFixedSize(32, 21);
    playSpeed_button->setFlat(true);
    connect(playSpeed_button, &QPushButton::clicked, this, [=] {
        QPoint pos;
        pos.setX(0);
        QMenu *playSpeed_menu = new QMenu(this);
        QAction *playSpeed_action[5];
        QString playSpeed_str[5] = {"×0.5", "×1", "×2", "×4", "×8"};
        for(int i=4; i>=0; i--) {
            playSpeed_action[i] = playSpeed_menu->addAction(playSpeed_str[i]);
            playSpeed_action[i]->setCheckable(false);
            connect(playSpeed_action[i], &QAction::triggered, this, [=] {
                if(currentPlaySpeed != pow(2, i-1)) {
                    playSpeed_button->setText(playSpeed_str[i]);
                    currentPlaySpeed = pow(2, i-1);
                    emit playSpeed_signal(pow(2, i-1));
                }
            });
        }
        pos.setY(-playSpeed_menu->sizeHint().height());
        playSpeed_menu->exec(playSpeed_button->mapToGlobal(pos));
    });

    //音量控制
    volumeShow_button = new QToolButton(this);
    volumeShow_button->setObjectName("volumeShow");
    volumeShow_button->setIcon(QIcon(":/icon/volume1.png"));
    volumeControl = new QWidget(parent);
    volumeControl->setObjectName("volumeControl");
    volumeControl->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    volumeControl->setVisible(false);

    volumeControl->setAttribute(Qt::WA_StyledBackground, true);
    volumeControl->setObjectName("volumeControl");
    //加载qss样式文件
    QFile afile(":/qss/volumeControl.qss");
    afile.open(QFile::ReadOnly);
    QTextStream afiletext(&afile);
    QString astylesheet = afiletext.readAll();
    volumeControl->setStyleSheet(astylesheet);
    file.close();

    volume_slider = new QSlider(volumeControl);
    volume_slider->setMaximum(100);
    volume_slider->setMinimum(0);
    volume_slider->setValue(50);
    volume_button = new QToolButton(volumeControl);
    volume_button->setObjectName("volume");
    volume_button->setIcon(QIcon(":/icon/volume1.png"));

    QVBoxLayout *volume_layout = new QVBoxLayout;
    volume_layout->addWidget(volume_slider);
    volume_layout->addWidget(volume_button);

    volumeControl->setLayout(volume_layout);

    //静音
    connect(volume_button, &QToolButton::clicked, this, [=] {
        if(isMuted) {
            volume_slider->setValue(volume_int);
        }
        else {
            volume_int = volume_slider->value();
            volume_slider->setValue(0);
        }
        isMuted = !isMuted;
    });
    //改变音量
    connect(volumeShow_button, &QToolButton::clicked, this, [=] {
        if(volumeControl->isVisible()) volumeControl->setVisible(false);
        else volumeControl->setVisible(true);
    });
    connect(volume_slider, &QSlider::valueChanged, this, [=] {
        emit volume_signal(volume_slider->value());
    });

    //音频波形图
    volumeGraphy_button = new QToolButton(this);
    volumeGraphy_button->setObjectName("volumeGraphy");
    volumeGraphy_button->setIcon(QIcon(":/icon/volumeGraphy.png"));
    connect(volumeGraphy_button, &QToolButton::clicked, this, [=] {
        emit volumeGraphy_signal();
    });

    //改变屏幕大小
    fullScreen_button = new QToolButton(this);
    fullScreen_button->setObjectName("fullScreen");
    fullScreen_button->setIcon(QIcon(":/icon/fullScreen1.png"));
    connect(fullScreen_button, &QToolButton::clicked, this, [=] {
        emit fullScreen_signal();
    });
    //显示列表
    showList_button = new QToolButton(this);
    showList_button->setObjectName("showList");
    showList_button->setIcon(QIcon(":/icon/showList_visible.png"));
    connect(showList_button, &QToolButton::clicked, this, [=] {
        emit showList_signal();
    });

    //    框架
    QVBoxLayout *main_layout = new QVBoxLayout;//主框架
    QHBoxLayout *wave_layout = new QHBoxLayout;
    progressBar_layout = new QHBoxLayout;  // 放进度条
    QHBoxLayout *control_layout = new QHBoxLayout;  //放按钮等控件
    QHBoxLayout *left_layout = new QHBoxLayout;
    QHBoxLayout *center_layout = new QHBoxLayout;
    QHBoxLayout *right_layout = new QHBoxLayout;

    main_layout->setContentsMargins(5, 0, 5, 5);
    wave_layout->setContentsMargins(0, 0, 0, 0);
    progressBar_layout->setContentsMargins(0, 0, 0, 0);
    control_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->setContentsMargins(0, 0, 0, 0);
    center_layout->setContentsMargins(0, 0, 0, 0);
    right_layout->setContentsMargins(0, 0, 0, 0);

    //布局
    main_layout->addLayout(wave_layout);
    main_layout->addLayout(progressBar_layout);
    main_layout->addLayout(control_layout);
    control_layout->addLayout(left_layout);
    control_layout->addStretch();
    control_layout->addLayout(center_layout);
    control_layout->addStretch();
    control_layout->addLayout(right_layout);

    //    左边部分
    left_layout->addWidget(changeMediaDirShow_button);// 媒体目录
    //    中间部分
    center_layout->addStretch();
    center_layout->addWidget(volumeGraphy_button, 1);//显示波形图
    center_layout->addSpacing(10);
    center_layout->addWidget(playSpeed_button, 1);  //倍速播放
    center_layout->addSpacing(10);
    center_layout->addWidget(playMode_button, 1);  //播放顺序
    center_layout->addSpacing(10);
    center_layout->addWidget(durationStep_button, 1);  //快进快退帧数
    center_layout->addSpacing(10);
    center_layout->addWidget(preOne_button, 1);//控制上一首
    center_layout->addWidget(playStatus_button, 1);  //控制播放状态
    center_layout->addWidget(nextOne_button, 1);  //控制下一首
    center_layout->addSpacing(10);
    center_layout->addWidget(volumeShow_button, 1);   //声音控制
    center_layout->addStretch();
    //    右边部分
    right_layout->addWidget(fullScreen_button, 1);   //全屏播放
    right_layout->addWidget(showList_button, 1); //显示播放列表

    progressBar_layout->addWidget(duration_slider);
    progressBar_layout->addWidget(duration_label);

    setLayout(main_layout);
}

QToolButton *PlayerControls::getChangeMediaDirShow_button() const
{
    return changeMediaDirShow_button;
}

QToolButton *PlayerControls::getPlayStatus_button() const
{
    return playStatus_button;
}

QToolButton *PlayerControls::getPreOne_button() const
{
    return preOne_button;
}

QToolButton *PlayerControls::getNextOne_button() const
{
    return nextOne_button;
}

QToolButton *PlayerControls::getVolume_button() const
{
    return volume_button;
}

QToolButton *PlayerControls::getFullScreen_button() const
{
    return fullScreen_button;
}

QToolButton *PlayerControls::getShowList_button() const
{
    return showList_button;
}

QLabel *PlayerControls::getDuration_label() const
{
    return duration_label;
}

DurationSlider *PlayerControls::getDuration_slider() const
{
    return duration_slider;
}

QSlider *PlayerControls::getVolume_slider() const
{
    return volume_slider;
}

QWidget *PlayerControls::getVolumeControl() const
{
    return volumeControl;
}

QToolButton *PlayerControls::getVolumeShow_button() const
{
    return volumeShow_button;
}

QToolButton *PlayerControls::getVolumeGraphy_button() const
{
    return volumeGraphy_button;
}

