#include "mediadir.h"

MediaDir::MediaDir(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    //加载qss样式文件
    QFile file(":/qss/mediaDir.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    setStyleSheet(stylesheet);
    file.close();

    //控件定义
    //媒体目录
    mediaDir_listWidget = new QListWidget(this);
    mediaDir_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    dirName_lineEdit = new QLineEdit(mediaDir_listWidget);
    dirName_lineEdit->setVisible(false);
    dirName_lineEdit->grabKeyboard();
    dirName_lineEdit->releaseKeyboard();
    mediaItem_tableView = new QTableView(this);
    mediaItem_tableView->verticalHeader()->hide();
    mediaItem_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mediaItem_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mediaItem_tableView->setFocusPolicy(Qt::NoFocus);
    mediaDir_label = new QLabel("音视频库", this);
    addMediaDir_button = new QPushButton(this);
    addMediaDir_button->setObjectName("addMediaDir_button");
    addMediaDir_button->setIcon(QIcon(":/icon/addMediaDir.svg"));
    addMediaDir_button->resize(30, 30);
    addMediaDir_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mediaItem_sqlQueryModel = new QSqlQueryModel(this);

    //媒体目录项
    mediaItem_label = new QLabel("新建目录", this);
    mediaItem_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    playAll_button = new QPushButton(this);
    playAll_button->setIcon(QIcon(":/icon/playAll.svg"));
    playAll_button->setText("播放全部");
    playAll_button->resize(70, 70);
    playAll_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    close_button = new QPushButton(this);
    close_button->setIcon(QIcon(":/icon/close.svg"));
    close_button->resize(30, 30);
    close_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    close_button->setObjectName("close_button");

    //控件布局
    QHBoxLayout *layout_topleft = new QHBoxLayout();
    layout_topleft->setContentsMargins(0, 0, 0, 0);
    layout_topleft->addWidget(mediaDir_label);
    layout_topleft->addSpacing(10);
    layout_topleft->addWidget(addMediaDir_button);

    QVBoxLayout *layout_left = new QVBoxLayout();
    layout_left->setContentsMargins(0, 0, 0, 0);
    layout_left->addLayout(layout_topleft, 1);
    layout_left->addWidget(mediaDir_listWidget, 9);

    left_widget = new QWidget(this);
    left_widget->setLayout(layout_left);
    left_widget->setObjectName("left_widget");

    QHBoxLayout *layout_tabletop = new QHBoxLayout();
    layout_tabletop->setContentsMargins(0, 0, 0, 0);
    layout_tabletop->addWidget(close_button);
    layout_tabletop->addWidget(mediaItem_label);
    layout_tabletop->addWidget(playAll_button);

    QVBoxLayout *layout_table = new QVBoxLayout();
    layout_table->setContentsMargins(0, 0, 0, 0);
    layout_table->addLayout(layout_tabletop, 1);
    layout_table->addWidget(mediaItem_tableView, 9);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(left_widget, 1);
    layout->addLayout(layout_table, 4);

    setLayout(layout);

    //信号与槽函数
    //添加目录
    connect(addMediaDir_button, &QPushButton::clicked, this, &MediaDir::on_addMediaDir_button_clicked);
    connect(dirName_lineEdit, &QLineEdit::editingFinished, this, &MediaDir::addMediaDir);
    //右键菜单栏
    connect(mediaDir_listWidget, &QListWidget::customContextMenuRequested, this, &MediaDir::on_mediaDir_menu);
    //播放全部
    connect(playAll_button, &QPushButton::clicked, this, [=] {
        QList<QString> pathes;
        for(int i=0; i<mediaItem_sqlQueryModel->rowCount(); i++) {
            qDebug() << i << " " << mediaItem_sqlQueryModel->index(i, 1).data().toString();
            pathes.append(mediaItem_sqlQueryModel->index(i, 1).data().toString());
        }
        emit on_playAll_button_clicked(pathes);
    });
    //目录项右键菜单栏
    connect(mediaItem_tableView, &QTableView::customContextMenuRequested, this, &MediaDir::on_mediaItem_menu);
    //隐藏媒体库
    connect(close_button, &QPushButton::clicked, this, [=] {
        emit close_button_clicked();
    });
}

MediaDir::~MediaDir()
{

}

void MediaDir::on_addMediaDir_button_clicked()
{
    addMediaDir_button->setEnabled(false);
    dirName_lineEdit->move(0, 10);
    dirName_lineEdit->setText("新建目录");
    dirName_lineEdit->setVisible(true);
    dirName_lineEdit->grabKeyboard();
    mediaDir_listWidget->insertItem(0, "新建目录");
}

void MediaDir::showMediaItem(QListWidgetItem *item)
{
    QString dirname = item->data(0).toString();
    mediaItem_label->setText(dirname);
    QString str = QString("SELECT name as 文件名, path as 路径, type as 类型 FROM MediaItem WHERE dirname = '%1'").arg(dirname);
    mediaItem_sqlQueryModel->setQuery(str);
    mediaItem_tableView->setModel(mediaItem_sqlQueryModel);
    mediaItem_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mediaItem_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    mediaItem_tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    mediaItem_tableView->setColumnWidth(0, 100);
    mediaItem_tableView->setColumnWidth(2, 50);
}

void MediaDir::addMediaDir()
{
    QString dirname = dirName_lineEdit->text();
    if(dirname == "") {
        QMessageBox::critical(NULL, "InfinityPlayer", "目录名不能为空", QMessageBox::Ok);
        dirName_lineEdit->setVisible(false);
        dirName_lineEdit->releaseKeyboard();
        mediaDir_listWidget->takeItem(0);
        addMediaDir_button->setEnabled(true);
        return;
    }
    dirName_lineEdit->setVisible(false);
    dirName_lineEdit->releaseKeyboard();
    if(infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
        QListWidgetItem *item = mediaDir_listWidget->item(0);
        item->setText(dirname);
        mediaDir_listWidget->setCurrentRow(0);
        showMediaItem(mediaDir_listWidget->currentItem());
    }
    else {
        dirname += "_0";
        while(!infinityPlayer_sqlQuery->exec(QString("INSERT INTO MediaDir VALUES('%1')").arg(dirname))) {
            dirname += "_0";
        }
        QListWidgetItem *item = mediaDir_listWidget->item(0);
        item->setText(dirname);
        mediaDir_listWidget->setCurrentRow(0);
        showMediaItem(mediaDir_listWidget->currentItem());
    }
    addMediaDir_button->setEnabled(true);
}

void MediaDir::delMediaDir()
{
    if(mediaDir_listWidget->count() == 1) {
        QMessageBox::critical(NULL, "InfinityPlayer", "请至少保留一个目录", QMessageBox::Ok);
        return;
    }
    QListWidgetItem *item = mediaDir_listWidget->currentItem();
    QString dirname = item->data(0).toString();
    QString sql = QString("delete from MediaItem where dirname = '%1'").arg(dirname);
    QMessageBox::StandardButton result = QMessageBox::question(NULL, "InfinityPlayer", "确定要删除目录\"" + dirname + "\"吗?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes) {
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
}

void MediaDir::on_mediaDir_menu(const QPoint &pos)
{
    std::shared_ptr<QMenu> menu = std::make_shared<QMenu>();
    QAction *del = menu->addAction(tr("删除"));
    QAction *rename = menu->addAction(tr("重命名"));
    //删除目录
    connect(del, &QAction::triggered, this, &MediaDir::delMediaDir);
    //重命名目录
    connect(rename, &QAction::triggered, this, [=] {
        isRename = true;
        preName = mediaDir_listWidget->currentItem()->text();
        mediaDir_listWidget->currentItem()->setFlags(mediaDir_listWidget->currentItem()->flags() | Qt::ItemIsEditable);
        mediaDir_listWidget->editItem(mediaDir_listWidget->currentItem());
        connect(mediaDir_listWidget, &QListWidget::itemChanged, this, &MediaDir::renameMediaDir);
    });
    if (mediaDir_listWidget->itemAt(pos) != nullptr)
    {
        menu->exec(mediaDir_listWidget->mapToGlobal(pos));
    }
}

void MediaDir::renameMediaDir(QListWidgetItem *item)
{
    if(isRename) {
        isRename = false;
        if(item->text() == "") {
            QMessageBox::critical(NULL, "InfinityPlayer", "目录名不能为空", QMessageBox::Ok);
            item->setText(preName);
            return;
        }
        if(preName == item->text()) return;
        disconnect(mediaDir_listWidget, &QListWidget::itemChanged, this, &MediaDir::renameMediaDir);
        QString newName = item->text();
        if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaDir SET dirname = '%1' WHERE dirname = '%2'").arg(newName, preName))) {
            if(infinityPlayer_sqlQuery->exec(QString("UPDATE MediaItem SET dirname = '%1' WHERE dirname = '%2'").arg(newName, preName))) {
                mediaItem_label->setText(newName);
            }
            else {
                QMessageBox::critical(NULL, "InfinityPlayer", "目录\"" + newName + "\"已存在", QMessageBox::Ok);
                item->setText(preName);
            }
        }
        else {
            QMessageBox::critical(NULL, "InfinityPlayer", "目录\"" + newName + "\"已存在", QMessageBox::Ok);
            item->setText(preName);
        }
    }
}

void MediaDir::addMediaItem()
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
                QMessageBox::critical(NULL, "InfinityPlayer", "添加失败", QMessageBox::Ok);
            }
        }
        else {
            QMessageBox::critical(NULL, "InfinityPlayer", "不可重复添加", QMessageBox::Ok);
        }
    }
}

void MediaDir::delMediaItem()
{
    QModelIndex index = mediaItem_tableView->currentIndex();
    QString path = mediaItem_sqlQueryModel->index(index.row(), 1).data().toString();
    QUrl p(path);
    QString dirname = mediaItem_label->text();
    QMessageBox::StandardButton result = QMessageBox::question(NULL, "InfinityPlayer", "确定要删除文件\"" + p.fileName() + "\"吗?", QMessageBox::Yes | QMessageBox::No);
    if(result == QMessageBox::Yes) {
        QString sql = QString("DELETE FROM MediaItem WHERE dirname = '%1' and path = '%2'").arg(dirname, path);
        if(infinityPlayer_sqlQuery->exec(sql)) {
            showMediaItem(mediaDir_listWidget->currentItem());
        }
        else {
            qDebug() << "删除失败!";
        }
    }
}

void MediaDir::on_mediaItem_menu(const QPoint &pos)
{
    std::shared_ptr<QMenu> menu = std::make_shared<QMenu>();
    QAction *add = menu->addAction(tr("添加"));
    QAction *del = menu->addAction(tr("删除"));
    QAction *addToPlayList = menu->addAction(tr("添加至播放列表"));
    //添加目录项
    connect(add, &QAction::triggered, this, &MediaDir::addMediaItem);
    //删除目录项
    connect(del, &QAction::triggered, this, &MediaDir::delMediaItem);
    //添加至播放列表
    connect(addToPlayList, &QAction::triggered, this, [=] {
        int row = mediaItem_tableView->currentIndex().row();
        QString path = mediaItem_sqlQueryModel->index(row, 1).data().toString();
        emit addToPlayList_triggered(path);
    });
    if (mediaItem_tableView->indexAt(pos).row() == -1)
    {
        del->setEnabled(false);
        addToPlayList->setEnabled(false);
    }
    menu->exec(mediaItem_tableView->mapToGlobal(pos));
}

QSqlQuery *MediaDir::getInfinityPlayer_sqlQuery() const
{
    return infinityPlayer_sqlQuery;
}

QListWidget *MediaDir::getMediaDir_listWidget() const
{
    return mediaDir_listWidget;
}

QTableView *MediaDir::getMediaItem_tableView() const
{
    return mediaItem_tableView;
}

QSqlQueryModel *MediaDir::getMediaItem_sqlQueryModel() const
{
    return mediaItem_sqlQueryModel;
}

void MediaDir::setInfinityPlayer_sqlQuery(QSqlQuery *newInfinityPlayer_sqlQuery)
{
    infinityPlayer_sqlQuery = newInfinityPlayer_sqlQuery;
}

QLabel *MediaDir::getMediaItem_label() const
{
    return mediaItem_label;
}
