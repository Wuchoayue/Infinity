#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QWidget>
#include <qdebug.h>
#include <player.h>
#include <QImage>
#include <QLabel>

class Thumbnail : public QWidget
{
    Q_OBJECT

public:
    explicit Thumbnail(QWidget *parent = nullptr);
    QPixmap* pixmap = nullptr;
    QLabel* pic  = nullptr;
    QLabel* time = nullptr;
};

#endif // THUMBNAIL_H
