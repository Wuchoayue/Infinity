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
#include <QDir>
#include <QMessageBox>
#include <QCryptographicHash>
#include "mediainfo.h"

struct Element {
public:
    Element(QString cover_, QString name_, QString path_, QByteArray md5_) {
        cover = cover_;
        name = name_;
        path = path_;
        md5 = md5_;
    }
    QString cover;  //封面地址
    QString name;   //文件名
    QString path;   //路径
    QByteArray md5; //文件加密值

};

class PlayListModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit PlayListModel(QObject* parent = 0);
    ~PlayListModel();
    QString media(const QModelIndex &index);
    QByteArray mediaMd5(const QModelIndex &index);
    QList<Element> totalMedia();   //返回播放列表中所有文件
    bool haveMedia(QUrl path);   //播放列表是否包含该路径
    int rowOfPath(QString path);    //路径的索引
    void insert(const QUrl &path); //插入播放列表
    void remove(QList<QModelIndex> indexes);  //从播放列表中删除
    void removeOne(QModelIndex index);  //从播放列表中删除一个项
    void showMedia(QModelIndex &index); //查看音视频信息
    void insertAll(QUrl path, QString cover, QByteArray md5);   //加载时进行插入
    void clear();   //清空播放列表

signals:
    void changePlayList();

private:
    QList<Element> playList;
    QSet<QUrl> playList_set;
    QHash<QString, int> pathTorow;
    QList<QString> video_type;
    QList<QString> audio_type;
    QHash<QString, QString> nameTotype;
};

#endif // PLAYLISTMODEL_H
