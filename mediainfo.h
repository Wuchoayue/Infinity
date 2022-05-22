#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include "player.h"

class MediaInfo : public QWidget
{
    Q_OBJECT

public:
    explicit MediaInfo(QWidget *parent = 0);
    ~MediaInfo();
    void setInfo(VideoInfo videoInfo);  //展示视频信息
    void setInfo(AudioInfo audioInfo);  //展示音频信息

private:
    QTableWidget *info;
};

#endif // MEDIAINFO_H
