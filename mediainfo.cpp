#include "mediainfo.h"

MediaInfo::MediaInfo(QObject *parent)
    : QObject(parent)
{

}

MediaInfo::~MediaInfo()
{
    if(player) player->stop();
}

void MediaInfo::getCover(QUrl path)
{
    player = new QMediaPlayer(this);
    player->setSource(path);
    if(!player->isAvailable()) {
        qDebug()<<"grab failed";
    }
    else if(player->mediaStatus() == QMediaPlayer::InvalidMedia) {
        qDebug()<<"Media Invalid";
    }
    QVideoSink* sink = new QVideoSink(player);
    player->setVideoSink(sink);
    player->play();
    connect(sink, &QVideoSink::videoFrameChanged,this, [=](const QVideoFrame& frame){
        if(frame.isValid()) {
            if(!this->isStop){
                QVideoFrame targetframe(frame);
                if(targetframe.map(QVideoFrame::ReadOnly)){
                    QImage image = targetframe.toImage();
                    image = image.scaled(960,720);
                    emit getImage(image);
                    targetframe.unmap();
                    isStop = true;
                    player->stop();
                }
            }
        }
    });
}
