#include "playercontrols.h"
#include <QToolButton>
#include <QSlider>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
{
    preOne_button = new QToolButton(this);
    preOne_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    playStatus_button = new QToolButton(this);
    playStatus_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    nextOne_button = new QToolButton(this);
    nextOne_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    volume_slider = new QSlider(this);
    volume_slider->setMaximum(100);
    volume_slider->setMinimum(0);
    volume_slider->setValue(50);
    duration_slider = new DurationSlider(this);
    duration_slider->setOrientation(Qt::Horizontal);
    duration_slider->setMinimum(0);
    changeMediaDirShow_button = new QToolButton(this);
    changeMediaDirShow_button->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
    QBoxLayout *layout_bottom = new QHBoxLayout;
    layout_bottom->setContentsMargins(0, 0, 0, 0);
    layout_bottom->addWidget(changeMediaDirShow_button, 1);
    layout_bottom->addWidget(preOne_button, 1);
    layout_bottom->addWidget(playStatus_button, 1);
    layout_bottom->addWidget(nextOne_button, 1);
    layout_bottom->addWidget(volume_slider, 1);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(duration_slider, 1);
    layout->addLayout(layout_bottom, 1);
    setLayout(layout);
    //掐换播放状态
    connect(playStatus_button, &QToolButton::clicked, this, [=] {
        emit playStatus_signal();
    });
    //上一首
    connect(preOne_button, &QToolButton::clicked, this, [=] {
        emit preOne_signal();
    });
    connect(nextOne_button, &QToolButton::clicked, this, [=] {
        emit nextOne_signal();
    });
    connect(volume_slider, &QSlider::valueChanged, this, [=] {
        int value = volume_slider->value();
        emit volume_signal(value);
    });
    connect(duration_slider, &DurationSlider::sliderMoved, this, [&](int value) {
        emit duration_signal(value);
    });
    connect(changeMediaDirShow_button, &QToolButton::clicked, this, [=] {
        emit changeMediaDirShow_signal();
    });
}
