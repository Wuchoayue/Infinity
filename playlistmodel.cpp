#include "playlistmodel.h"

PlayListModel::PlayListModel(QObject *parent) : QStandardItemModel(parent)
{

}

PlayListModel::~PlayListModel()
{

}

QString PlayListModel::media(const QModelIndex &index)
{
    return playList[index.row()].path;
}

QList<QString> PlayListModel::totalMedia()
{
    QList<QString> pathes;
    for(int i=0; i<playList.size(); i++) {
        pathes.append(playList[i].path);
    }
    return pathes;
}

bool PlayListModel::haveMedia(QUrl path)
{
    return playList_set.contains(path);
}

int PlayListModel::rowOfPath(QString path)
{
    return pathTorow[path];
}

void PlayListModel::insert(const QUrl &path)
{
    //判断是否有重复
    if(playList_set.contains(path)) {
        qDebug() << "不可重复添加!";
    }
    else {
        playList_set.insert(path);
        pathTorow.insert(path.toString(), playList.size());
        Element ele(QIcon("E:/CourseProject/test.png"), path.fileName(), path.toString());
        playList.append(ele);
        QStandardItem *item = new QStandardItem(QIcon("E:/CourseProject/test.png"), path.fileName());
        insertRow(rowCount(), item);
    }
}

void PlayListModel::remove(QModelIndex &index)
{
    playList_set.remove(playList[index.row()].path);
    pathTorow.remove(playList[index.row()].path);
    playList.removeAt(index.row());
    removeRow(index.row());
}

void PlayListModel::showMedia(QModelIndex &index)
{
    qDebug() << playList[index.row()].path;
}

