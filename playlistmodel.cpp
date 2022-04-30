#include "playlistmodel.h"

PlayListModel::PlayListModel(QObject *parent) : QStandardItemModel(parent)
{
    m_index = -1;
}

PlayListModel::~PlayListModel()
{

}

void PlayListModel::insert(QUrl url)
{
    QList<int> a;
    a.append(0);
    a.append(1);
    a.append(2);
    qDebug() << a[1];
    a.removeAt(1);
    qDebug() << a[2];
    QStandardItem *item = new QStandardItem(QIcon("E:/CourseProject/test.png"), url.fileName());
    insertRow(rowCount(), item);
}
