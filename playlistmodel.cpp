#include "playlistmodel.h"

PlayListModel::PlayListModel(QObject *parent) : QStandardItemModel(parent)
{
    video_type.append(".mp4");
    video_type.append(".avi");
    video_type.append(".flv");
    video_type.append(".mov");
    audio_type.append(".mp3");
    audio_type.append(".wav");
    nameTotype.insert(".mp4", "MP4");
    nameTotype.insert(".avi", "AVI");
    nameTotype.insert(".flv", "FLV");
    nameTotype.insert(".mp3", "MP3");
    nameTotype.insert(".wav", "WAV");
    nameTotype.insert(".mov", "MOV");
}

PlayListModel::~PlayListModel()
{

}

QString PlayListModel::media(const QModelIndex &index)
{
    return playList[index.row()].path;
}

QByteArray PlayListModel::mediaMd5(const QModelIndex &index)
{
    return playList[index.row()].md5;
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
        QMessageBox::critical(NULL, "InfinityPlayer", "文件\"" + path.fileName() + "\"已存在", QMessageBox::Ok);
    }
    else {
        playList_set.insert(path);
        pathTorow.insert(path.toString(), playList.size());
        QString cover = QString("../playListIcon/%1_%2.png").arg(path.fileName().split(".")[0]).arg(rand());
        QFile f(path.toString());
        f.open(QFile::ReadOnly);
        QCryptographicHash md(QCryptographicHash::Md5);
        md.addData(&f);
        QByteArray md5 = md.result().toHex();
        f.close();
        VideoInfo videoInfo;
        AudioInfo audioInfo;
        if(DrawJPG(path.toString().toStdString().c_str(), cover.toStdString().c_str())) {
            Element ele(cover, path.fileName(), path.toString(), md5);
            playList.append(ele);
            QStandardItem *item = new QStandardItem(QIcon(cover), path.fileName());
            insertRow(rowCount(), item);
        }
        else {
            Element ele(":/icon/default.svg", path.fileName(), path.toString(), md5);
            playList.append(ele);
            QStandardItem *item = new QStandardItem(QIcon(":/icon/default.svg"), path.fileName());
            insertRow(rowCount(), item);
        }
    }
    emit changePlayList();
}

void PlayListModel::remove(QList<QModelIndex> indexes)
{
    for(int i=0; i<indexes.size(); i++) {
        QModelIndex curIndex = indexes[i];
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
    emit changePlayList();
}

void PlayListModel::removeOne(QModelIndex index)
{
    int row = index.row();
    playList_set.remove(playList[row].path);
    QHash<QString, int>::iterator iter = pathTorow.begin();
    pathTorow.remove(playList[row].path);
    while(iter != pathTorow.end()) {
        if(iter.value() > row) {
            iter.value() -= 1;
        }
        qDebug() << iter.key() << " " << iter.value();
        ++iter;
    }
    QFile file("../PlayListIcon/" + playList[row].cover);
    file.remove();
    playList.removeAt(row);
    removeRow(row);
    emit changePlayList();
}

void PlayListModel::showMedia(QModelIndex &index)
{
    MediaInfo *info = new MediaInfo();
    QString path = playList[index.row()].path;
    QString name = QUrl(path).fileName();
    info->setWindowIcon(QIcon(playList[index.row()].cover));
    info->setWindowTitle(name + " 详细信息");
    VideoInfo videoInfo;
    AudioInfo audioInfo;
    GetInfo(path.toStdString().c_str(), &videoInfo, &audioInfo);
    if(video_type.contains(name.right(name.length() - name.indexOf(".")))) {
        QFileInfo i(path);
        videoInfo.name = name;
        videoInfo.type = nameTotype[name.right(name.length() - name.indexOf("."))] + " 文件";
        videoInfo.path = i.absolutePath();
        videoInfo.size = i.size() < 1024 * 1024 * 0.1? QString::number(i.size() / 1024.0, 'f', 1) + " KB" : QString::number(i.size() / 1024.0 / 1024.0, 'f', 1) + " MB";
        videoInfo.a_bit_rate = audioInfo.bit_rate;
        videoInfo.sample_rate = audioInfo.sample_rate;
        videoInfo.channels = audioInfo.channels;
        info->setInfo(videoInfo);
    }
    else if (audio_type.contains(name.right(name.length() - name.indexOf(".")))){
        QFileInfo i(path);
        audioInfo.name = name;
        audioInfo.type = nameTotype[name.right(name.length() - name.indexOf("."))] + " 文件";
        audioInfo.path = i.absolutePath();
        audioInfo.size = i.size() < 1024 * 1024 * 0.1? QString::number(i.size() / 1024.0, 'f', 1) + " KB" : QString::number(i.size() / 1024.0 / 1024.0, 'f', 1) + " MB";
        info->setInfo(audioInfo);
    }
}

void PlayListModel::insertAll(QUrl path, QString cover, QByteArray md5)
{
    playList_set.insert(path);
    pathTorow.insert(path.toString(), playList.size());
    Element ele(cover, path.fileName(), path.toString(), md5);
    playList.append(ele);
    QStandardItem *item = new QStandardItem(QIcon(cover), path.fileName());
    insertRow(rowCount(), item);
    emit changePlayList();
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
    emit changePlayList();
}

bool PlayListModel::isAudio(QString path)
{
    QUrl p(path);
    QString name = p.fileName();
    return audio_type.contains(name.right(name.length() - name.indexOf(".")));
}
