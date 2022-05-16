#ifndef MEDIADIR_H
#define MEDIADIR_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>
#include <QLabel>
#include <QTableView>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QMenu>
#include <QMessageBox>

class MediaDir : public QWidget
{
    Q_OBJECT

public:
    explicit MediaDir(QWidget *parent = 0);
    ~MediaDir();
    //添加目录按钮被点击
    void on_addMediaDir_button_clicked();
    //显示目录项
    void showMediaItem(QListWidgetItem *item);
    //添加目录
    void addMediaDir();
    //删除目录
    void delMediaDir();
    //右键菜单栏
    void on_mediaDir_menu(const QPoint &pos);
    //重命名
    void renameMediaDir(QListWidgetItem *item);
    //添加目录项
    void addMediaItem();
    //删除目录项
    void delMediaItem();
    //目录项菜单
    void on_mediaItem_menu(const QPoint &pos);

public:
    QSqlQuery *getInfinityPlayer_sqlQuery() const;
    QListWidget *getMediaDir_listWidget() const;
    QTableView *getMediaItem_tableView() const;
    QSqlQueryModel *getMediaItem_sqlQueryModel() const;
    void setInfinityPlayer_sqlQuery(QSqlQuery *newInfinityPlayer_sqlQuery);
    QLabel *getMediaItem_label() const;

signals:
    //播放全部按钮被点击
    void on_playAll_button_clicked(QList<QString> pathes);
    //添加至播放列表信号
    void addToPlayList_triggered(QString path);
    //关闭媒体库信号
    void close_button_clicked();

private:
    QSqlQuery *infinityPlayer_sqlQuery; //数据库sql
    QListWidget *mediaDir_listWidget;    //媒体库目录
    QLabel *mediaDir_label; //媒体库目录标签
    QPushButton *addMediaDir_button;    //添加目录按钮
    QLineEdit *dirName_lineEdit;    //添加目录
    QTableView *mediaItem_tableView; //目录项
    QSqlQueryModel *mediaItem_sqlQueryModel;    //目录项数据模型
    QLabel *mediaItem_label;    //目录项标签
    QPushButton *playAll_button;    //播放全部按钮
    QPushButton *close_button;  //关闭媒体库
    bool isRename = false;  //是否在重命名
    QString preName;
    QWidget *left_widget;
};

#endif // MEDIADIR_H
