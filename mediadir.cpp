#include "mediadir.h"

MediaDir::MediaDir(QWidget *parent)
    : QWidget(parent)
{
    //控件定义
    //媒体目录
    mediaDir_listWidget = new QListWidget(this);
    mediaDir_listWidget->setGridSize(QSize(mediaDir_listWidget->width(), mediaDir_listWidget->height() * 0.8));
    mediaDir_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    dirName_lineEdit = new QLineEdit(mediaDir_listWidget);
    dirName_lineEdit->resize(mediaDir_listWidget->gridSize());
    dirName_lineEdit->setVisible(false);
    dirName_lineEdit_1 = new QLineEdit(mediaDir_listWidget);
    dirName_lineEdit_1->resize(mediaDir_listWidget->gridSize());
    dirName_lineEdit_1->setVisible(false);
    mediaItem_tableView = new QTableView(this);
    mediaDir_label = new QLabel("音视频目录", this);
    addMediaDir_button = new QToolButton(this);
    addMediaDir_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));
    delMediaDir_button = new QToolButton(this);
    delMediaDir_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
    mediaItem_sqlQueryModel = new QSqlQueryModel(this);

    //媒体目录项
    mediaItem_label = new QLabel("粤语歌曲", this);
    addMediaItem_button = new QToolButton(this);
    addMediaItem_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarShadeButton));
    delMediaItem_button = new QToolButton(this);
    delMediaItem_button->setIcon(style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));

    //控件布局
    QHBoxLayout *layout_topleft = new QHBoxLayout;
    layout_topleft->setContentsMargins(0, 0, 0, 0);
    layout_topleft->addWidget(mediaDir_label);
    layout_topleft->addSpacing(10);
    layout_topleft->addWidget(addMediaDir_button);
    layout_topleft->addWidget(delMediaDir_button);

    QVBoxLayout *layout_left = new QVBoxLayout;
    layout_left->setContentsMargins(0, 0, 0, 0);
    layout_left->addLayout(layout_topleft, 1);
    layout_left->addWidget(mediaDir_listWidget, 6);

    QHBoxLayout *layout_tabletop = new QHBoxLayout;
    layout_tabletop->setContentsMargins(0, 0, 0, 0);
    layout_tabletop->addWidget(mediaItem_label);
    layout_tabletop->addSpacing(30);
    layout_tabletop->addWidget(addMediaItem_button);
    layout_tabletop->addWidget(delMediaItem_button);

    QVBoxLayout *layout_table = new QVBoxLayout;
    layout_table->setContentsMargins(0, 0, 0, 0);
    layout_table->addLayout(layout_tabletop, 1);
    layout_table->addWidget(mediaItem_tableView, 6);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(layout_left, 1);
    layout->addLayout(layout_table, 4);

    setLayout(layout);

    //信号与槽函数
    //添加目录
    connect(addMediaDir_button, &QToolButton::clicked, this, &MediaDir::on_addMediaDir_button_clicked);
    connect(dirName_lineEdit, &QLineEdit::editingFinished, this, [=] {
        emit dirName_lineEdit_editingFinished();
    });
    //删除目录
    connect(delMediaDir_button, &QToolButton::clicked, this, [=] {
        emit delMediaDir_button_clicked();
    });
    //右键菜单栏
    connect(mediaDir_listWidget, &QListWidget::customContextMenuRequested, this, &MediaDir::on_mediaDir_menu);
    //重命名
    connect(dirName_lineEdit_1, &QLineEdit::editingFinished, this, [=] {
        emit dirName_lineEdit_1_editingFinished();
    });
    //添加目录项
    connect(addMediaItem_button, &QToolButton::clicked, this, [=] {
        emit addMediaItem_button_clicked();
    });
    //删除目录项
    connect(delMediaItem_button, &QToolButton::clicked, this, [=] {
        emit delMediaItem_button_clicked();
    });
}

MediaDir::~MediaDir()
{

}

void MediaDir::on_addMediaDir_button_clicked()
{

    dirName_lineEdit->move(0, mediaDir_listWidget->count() * dirName_lineEdit->size().height());
    dirName_lineEdit->setText("新建目录");
    dirName_lineEdit->setVisible(true);
    dirName_lineEdit->grabKeyboard();
}

void MediaDir::showMediaItem(QListWidgetItem *item)
{
    QString dirname = item->data(0).toString();
    mediaItem_label->setText(dirname);
    QString str = QString("SELECT name as Name, path as Path, type as Type FROM MediaItem WHERE dirname = '%1'").arg(dirname);
    mediaItem_sqlQueryModel->setQuery(str);
    mediaItem_tableView->setModel(mediaItem_sqlQueryModel);
}

void MediaDir::addMediaDir(QSqlQuery *infinityPlayer_sqlQuery)
{
    QString dirname = dirName_lineEdit->text();
    dirName_lineEdit->setVisible(false);
    dirName_lineEdit->releaseKeyboard();
    if(infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
        mediaDir_listWidget->addItem(dirname);
    }
    else {
        dirname += "_0";
        while(!infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
            dirname += "_0";
        }
        mediaDir_listWidget->addItem(dirname);
        mediaDir_listWidget->setCurrentRow(mediaDir_listWidget->count() - 1);
        QListWidgetItem *item = mediaDir_listWidget->currentItem();
        showMediaItem(item);
    }
}

void MediaDir::delMediaDir(QSqlQuery *infinityPlayer_sqlQuery)
{
    if(mediaDir_listWidget->count() == 1) {
        qDebug() << "至少有一个目录!";
        return;
    }
    QListWidgetItem *item = mediaDir_listWidget->currentItem();
    QString dirname = item->data(0).toString();
    QString sql = QString("delete from MediaItem where dirname = '%1'").arg(dirname);
    if(infinityPlayer_sqlQuery->exec(sql)) {
        sql = QString("delete from MediaDir where dirname = '%1'").arg(dirname);
        if(infinityPlayer_sqlQuery->exec(sql)) {
            mediaDir_listWidget->takeItem(mediaDir_listWidget->currentRow());
            mediaDir_listWidget->setCurrentRow(0);
            QListWidgetItem *item = mediaDir_listWidget->currentItem();
            showMediaItem(item);
        }
        else {
            qDebug() << sql;
        }
    }
    else {
        qDebug() << sql;
    }
}

void MediaDir::on_mediaDir_menu(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    QAction* change = new QAction(tr("重命名"), menu);
    connect(change, &QAction::triggered, this, [=] {
        QListWidgetItem *item = mediaDir_listWidget->itemAt(pos);
        QModelIndex index = mediaDir_listWidget->indexAt(pos);
        QString oldname = item->data(0).toString();
        dirName_lineEdit_1->setText(oldname);
        dirName_lineEdit_1->move(0, index.row() * dirName_lineEdit_1->size().height());
        dirName_lineEdit_1->setVisible(true);
        dirName_lineEdit_1->grabKeyboard();
    });
    if (mediaDir_listWidget->itemAt(pos)!=nullptr)
    {
        menu->addAction(change);
        menu->popup(mediaDir_listWidget->mapToGlobal(pos));
    }
}

void MediaDir::renameMediaDir(QSqlQuery *infinityPlayer_sqlQuery)
{
    QListWidgetItem *item = mediaDir_listWidget->currentItem();
    QString oldname = item->data(0).toString();
    QString newname = dirName_lineEdit_1->text();
    if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaDir SET dirname = '%1' WHERE dirname = '%2'").arg(newname, oldname))) {
        if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaItem SET dirname = '%1' WHERE dirname = '%2'").arg(newname, oldname))) {
            item->setData(0, newname);
            dirName_lineEdit_1->setVisible(false);
            mediaItem_label->setText(newname);
        }
        else {
            qDebug() << "修改失败!";
        }
    }
    else {
        qDebug() << "修改失败!";
    }
    dirName_lineEdit_1->releaseKeyboard();
}

void MediaDir::addMediaItem(QSqlQuery *infinityPlayer_sqlQuery)
{
    QString path = QFileDialog::getOpenFileName(this,"open file",".","Audio (*.mp3 *.wav);;Video (*.mp4 *.avi *.flv)");
    if(path != "") {
        int a = path.lastIndexOf("/");
        int b = path.lastIndexOf(".");
        QListWidgetItem *item = mediaDir_listWidget->currentItem();
        QString dirname = item->data(0).toString();
        QString name = path.mid(a + 1, b - a - 1);
        QString type = path.right(path.length() - b - 1);
        QString sql0 = QString("SELECT * FROM MediaItem WHERE dirname = '%1' and path = '%2'").arg(dirname, path);
        infinityPlayer_sqlQuery->exec(sql0);
        if(!infinityPlayer_sqlQuery->next()) {
            QString sql = QString("INSERT INTO MediaItem VALUES('%1', '%2', '%3', '%4')").arg(dirname, name, path, type);
            if(infinityPlayer_sqlQuery->exec(sql)) {
                showMediaItem(item);
            }
            else {
                qDebug() << "插入失败!";
            }
        }
        else {
            qDebug() << "插入失败!";
        }
    }
}

void MediaDir::delMediaItem(QSqlQuery *infinityPlayer_sqlQuery)
{
    QModelIndex index = mediaItem_tableView->currentIndex();
    QString path = mediaItem_sqlQueryModel->index(index.row(), 1).data().toString();
    QString dirname = mediaItem_label->text();
    QString sql = QString("DELETE FROM MediaItem WHERE dirname = '%1' and path = '%2'").arg(dirname, path);
    if(infinityPlayer_sqlQuery->exec(sql)) {
        showMediaItem(mediaDir_listWidget->currentItem());
    }
    else {
        qDebug() << "删除失败!";
    }
}
