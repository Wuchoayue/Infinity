#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QList>
#include <QStandardItemModel>
#include <QImage>
#include <QUrl>
#include <QIcon>
#include <QStandardItem>
#include <QSet>
#include <QHash>
#include <QFile>
#include "mediainfo.h"

struct Element {
public:
    Element(QString cover_, QString name_, QString path_) {
        cover = cover_;
        name = name_;
        path = path_;
    }
    QString cover;
    QString name;
    QString path;
};

class PlayListModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit PlayListModel(QObject* parent = 0);
    ~PlayListModel();

public:
    QString media(const QModelIndex &index);
    QList<Element> totalMedia();   //返回播放列表中所有文件
    bool haveMedia(QUrl path);   //播放列表是否包含该路径
    int rowOfPath(QString path);    //路径的索引

public:
    void insert(const QUrl &path); //插入播放列表
    void remove(QList<QModelIndex> indexes);  //从播放列表中删除
    void showMedia(QModelIndex &index); //查看音视频信息
    void insertAll(QUrl path, QString iconPath);   //加载时进行插入

private:
    QList<Element> playList;
    QSet<QUrl> playList_set;
    QHash<QString, int> pathTorow;
};

#endif // PLAYLISTMODEL_H
