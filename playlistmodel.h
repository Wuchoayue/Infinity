#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QList>
#include <QStandardItemModel>
#include <QImage>
#include <QUrl>
#include <QIcon>
#include <QStandardItem>

struct Element {
public:
    Element(QImage cover_, QString name_) {
        cover = cover_;
        name = name_;
    }
    QImage cover;
    QString name;
};

class PlayListModel : public QStandardItemModel
{
    Q_OBJECT

public:

    explicit PlayListModel(QObject* parent = 0);
    ~PlayListModel();
    enum PlayMode {
        CurrentItemOnce, //只播放当前
        Sequential, //顺序播放
        Loop, //循环播放
        Random //随机播放
    };

public:
//    Element media(const QModelIndex &index) const;
//    void setPlaybackMode(PlayMode mode);
//    PlayMode playbackMode() const;
//    QModelIndex currentIndex() const;
//    void setPlayIndex(int index);

public:
    void insert(QUrl url); //插入播放列表

private:
    QList<Element> playList;
    PlayMode playMode;
    int m_index;

};

#endif // PLAYLISTMODEL_H
