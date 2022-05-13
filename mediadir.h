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
    void addMediaDir(QSqlQuery *infinityPlayer_sqlQuery);
    //删除目录
    void delMediaDir(QSqlQuery *infinityPlayer_sqlQuery);
    //右键菜单栏
    void on_mediaDir_menu(const QPoint &pos);
    //重命名
    void renameMediaDir(QSqlQuery *infinityPlayer_sqlQuery);
    //添加目录项
    void addMediaItem(QSqlQuery *infinityPlayer_sqlQuery);
    //删除目录项
    void delMediaItem(QSqlQuery *infinityPlayer_sqlQuery);

signals:
    //新建目录编辑完成信号
    void dirName_lineEdit_editingFinished();
    //删除目录按钮点击信号
    void delMediaDir_button_clicked();
    //重命名编辑完成信号
    void dirName_lineEdit_1_editingFinished();
    //添加目录项按钮点击信号
    void addMediaItem_button_clicked();
    //删除目录项按钮点击信号
    void delMediaItem_button_clicked();

public:
    QListWidget *mediaDir_listWidget;    //媒体库目录
    QLabel *mediaDir_label; //媒体库目录标签
    QToolButton *addMediaDir_button;    //添加目录按钮
    QToolButton *delMediaDir_button;    //删除目录按钮
    QLineEdit *dirName_lineEdit;    //添加目录
    QLineEdit *dirName_lineEdit_1;  //修改目录
    QTableView *mediaItem_tableView; //目录项
    QSqlQueryModel *mediaItem_sqlQueryModel;    //目录项数据模型
    QLabel *mediaItem_label;    //目录项标签
    QToolButton *addMediaItem_button;    //添加目录项按钮
    QToolButton *delMediaItem_button;    //删除目录项按钮
};

#endif // MEDIADIR_H
