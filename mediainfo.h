#ifndef MEDIAINFO_H
#define MEDIAINFO_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>

class MediaInfo : public QObject
{
    Q_OBJECT
public:
    explicit MediaInfo(QObject *parent = nullptr);
    ~MediaInfo();
    void getCover(QUrl path);    //获取封面

signals:
    void getImage(QImage img);

private:
    QMediaPlayer *player = nullptr;
    bool isStop = false;
};

#endif // MEDIAINFO_H
