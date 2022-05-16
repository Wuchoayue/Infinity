#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>

class MediaInfo : public QWidget
{
    Q_OBJECT

public:
    struct VideoInfo {
        QString name;   //文件名
        QString type;   //文件类型
        QString path;   //文件路径
        QString size;   //文件大小
        QString duration;   //时长
        QString bit_rate;   //视频码率
        QString frame_rate; //帧率
        QString coding_format;  //编码格式
        QString resolving;  //分辨率
    };
    struct AudioInfo {
        QString name;   //文件名
        QString type;   //文件类型
        QString path;   //文件路径
        QString size;   //文件大小
        QString duration;   //时长
        QString bit_rate;   //音频码率
        QString coding_format;  //编码格式
        QString channels;   //声道数
        QString album; //专辑信息
        QString singer;  //演唱者信息
    };

    explicit MediaInfo(QWidget *parent = 0);
    ~MediaInfo();
    void setInfo(VideoInfo videoInfo);  //展示视频信息
    void setInfo(AudioInfo audioInfo);  //展示音频信息

private:
    QTableWidget *info;
};

#endif // MEDIAINFO_H
