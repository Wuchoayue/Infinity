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

QList<Element> PlayListModel::totalMedia()
{
    return playList;
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
        if(path.fileName().split(".")[1] == "mp4") {
            MediaInfo *mediaInfo = new MediaInfo(this);
            mediaInfo->getCover(path);
            connect(mediaInfo, &MediaInfo::getImage, this, [=](QImage img) {
                QString cover = QString("../playListIcon/%1_%2.png").arg(path.fileName().split(".")[0]).arg(rand());
                img.save(cover);
                Element ele(cover, path.fileName(), path.toString());
                playList.append(ele);
                QStandardItem *item = new QStandardItem(QIcon(cover), path.fileName());
                insertRow(rowCount(), item);
            });
        }
        else {
            QString cover = QString("../playListIcon/%1_%2.png").arg(path.fileName().split(".")[0]).arg(rand());
            QImage img("E:/CourseProject/test.png");
            img.save(cover);
            Element ele(cover, path.fileName(), path.toString());
            playList.append(ele);
            QStandardItem *item = new QStandardItem(QIcon(cover), path.fileName());
            insertRow(rowCount(), item);
        }
    }
}

void PlayListModel::remove(QList<QModelIndex> indexes)
{
    for(int i=0; i<indexes.size(); i++) {
        QModelIndex curIndex = indexes[i];
        qDebug() << curIndex.row() << "  " << i;
        playList_set.remove(playList[curIndex.row()].path);
        pathTorow.remove(playList[curIndex.row()].path);
        QFile file("../PlayListIcon/" + playList[curIndex.row()].cover);
        file.remove();
    }
    for(int i=0; i<indexes.size(); i++) {
        QModelIndex curIndex = indexes[i];
        playList.removeAt(curIndex.row());
        removeRow(curIndex.row());
    }
    pathTorow.clear();
    for(int i=0; i<playList.size(); i++) {
        pathTorow.insert(playList[i].path, i);
    }
}

void PlayListModel::showMedia(QModelIndex &index)
{
    qDebug() << playList[index.row()].path;
}

void PlayListModel::insertAll(QUrl path, QString iconPath)
{
    playList_set.insert(path);
    pathTorow.insert(path.toString(), playList.size());
    Element ele(iconPath, path.fileName(), path.toString());
    playList.append(ele);
    QStandardItem *item = new QStandardItem(QIcon(iconPath), path.fileName());
    insertRow(rowCount(), item);
}

void PlayListModel::clear()
{
    removeRows(0, rowCount());
    playList.clear();
    playList_set.clear();
    pathTorow.clear();
    QDir dir("../PlayListIcon");
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach(QFileInfo file, fileList) {
        file.dir().remove(file.fileName());
    }
}

