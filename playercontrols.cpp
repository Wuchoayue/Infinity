#include "playercontrols.h"
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
{

    //进度条
    duration_label = new QLabel(this);
    duration_label->setText("00:00 / 00:00");
    duration_slider = new DurationSlider(this);
    duration_slider->setOrientation(Qt::Horizontal);
    duration_slider->setMinimum(0);

    //改变目录显示状态
    changeMediaDirShow_button = new QToolButton(this);
    changeMediaDirShow_button->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    connect(changeMediaDirShow_button, &QToolButton::clicked, this, [=] {
        emit changeMediaDirShow_signal();
    });

    //倒放
    invert_button = new QPushButton("倒", this);
    invert_button->setFlat(true);
    connect(invert_button, &QPushButton::clicked, this, [=] {
        emit invert_signal();
    });

    //进度变化幅度
    durationStep_bututon = new QPushButton("+1", this);
    durationStep_bututon->setFlat(true);
    connect(durationStep_bututon, &QPushButton::clicked, this, [=] {
        if(durationStep_value == 1) {
            durationStep_value = 8;
            durationStep_bututon->setText("+8");
        }
        else {
            durationStep_value = 1;
            durationStep_bututon->setText("+1");
        }
        emit durationStep_signal(durationStep_value);
    });

    //播放模式控制
    playMode_button = new QToolButton(this);
    playMode_button->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    connect(playMode_button, &QToolButton::clicked, this, [=] {
        QPoint pos;
        pos.setX(0);
        QMenu *playMode_menu = new QMenu(this);
        QAction *playMode_action[4];
        QString playMode_icon[4] = {"E:/CourseProject/test.png", "E:/CourseProject/test.png", "E:/CourseProject/test.png", "E:/CourseProject/test.png"};
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
    preOne_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(preOne_button, &QToolButton::clicked, this, [=] {
        emit preOne_signal();
    });

    //播放状态
    playStatus_button = new QToolButton(this);
    playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playStatus_button, &QToolButton::clicked, this, [=] {
        emit playStatus_signal();
    });

    //下一首
    nextOne_button = new QToolButton(this);
    nextOne_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(nextOne_button, &QToolButton::clicked, this, [=] {
        emit nextOne_signal();
    });

    //播放速度控制
    playSpeed_button = new QPushButton("倍速", this);
    playSpeed_button->setFlat(true);
    connect(playSpeed_button, &QPushButton::clicked, this, [=] {
        QPoint pos;
        pos.setX(0);
        QMenu *playSpeed_menu = new QMenu(this);
        QAction *playSpeed_action[4];
        QString playSpeed_str[3] = {"×1", "×2", "×4"};
        for(int i=2; i>=0; i--) {
            playSpeed_action[i] = playSpeed_menu->addAction(playSpeed_str[i]);
            playSpeed_action[i]->setCheckable(false);
            connect(playSpeed_action[i], &QAction::triggered, this, [=] {
                if(currentPlaySpeed != pow(2, i)) {
                    playSpeed_button->setText(playSpeed_str[i]);
                    currentPlaySpeed = pow(2, i);
                    emit playSpeed_signal(pow(2, i));
                }
            });
        }
        pos.setY(-playSpeed_menu->sizeHint().height());
        playSpeed_menu->exec(playSpeed_button->mapToGlobal(pos));
    });

    //音量控制
    volume_slider = new QSlider(this);
    volume_slider->setMaximum(100);
    volume_slider->setMinimum(0);
    volume_slider->setValue(50);
    volume_slider->setOrientation(Qt::Horizontal);
    volume_button = new QToolButton(this);
    volume_button->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    //静音
    connect(volume_button, &QToolButton::clicked, this, [=] {
        if(isMuted) {
            volume_slider->setValue(volume_int);
        }
        else {
            volume_slider->setValue(0);
        }
        isMuted = !isMuted;
    });
    //改变音量
    connect(volume_slider, &QSlider::valueChanged, this, [=] {
        int value = volume_slider->value();
        volume_int = value;
        emit volume_signal(value);
    });

    //音频波形图
    volumeGraphy_button = new QToolButton(this);
    volumeGraphy_button->setIcon(style()->standardIcon(QStyle::SP_DesktopIcon));
    connect(volumeGraphy_button, &QToolButton::clicked, this, [=] {
        emit volumeGraphy_signal();
    });

    //改变屏幕大小
    fullScreen_button = new QToolButton(this);
    fullScreen_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(fullScreen_button, &QToolButton::clicked, this, [=] {
        emit fullScreen_signal();
    });
    //显示列表
    showList_button = new QToolButton(this);
    showList_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    connect(showList_button, &QToolButton::clicked, this, [=] {
        isShowList = !isShowList;
        emit showList_signal(isShowList);
    });
    //    框架

    main_layout = new QVBoxLayout;//主框架
    wave_layout = new QHBoxLayout;
    progressBar_layout = new QHBoxLayout;  // 放进度条
    control_layout = new QHBoxLayout;  //放按钮等控件
    left_layout = new QHBoxLayout;
    center_layout = new QHBoxLayout;
    right_layout = new QHBoxLayout;
    main_layout->setContentsMargins(0, 0, 0, 0);
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
        center_layout->addWidget(preOne_button, 1);//控制上一首
        center_layout->addWidget(playStatus_button, 1);  //控制播放状态
        center_layout->addWidget(nextOne_button, 1);  //控制下一首
        center_layout->addWidget(volumeGraphy_button, 1);//显示波形图
        center_layout->addWidget(playSpeed_button, 1);  //倍速播放
        center_layout->addWidget(playMode_button, 1);  //播放顺序
        center_layout->addWidget(durationStep_bututon, 1);  //快进快退帧数
        center_layout->addWidget(invert_button, 1);  //倒序播放
        center_layout->addWidget(volume_button, 1);   //声音控制
        center_layout->addWidget(volume_slider, 1);   //声音控制
        center_layout->addStretch();
        //    右边部分
        right_layout->addWidget(fullScreen_button, 1);   //全屏播放
        right_layout->addWidget(showList_button, 1); //显示播放列表

        progressBar_layout->addWidget(duration_slider);
        progressBar_layout->addWidget(duration_label);

        setLayout(main_layout);

}


void PlayerControls::setShowListVisable(bool value)
{
    isShowList=value;
}
