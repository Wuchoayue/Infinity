#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QWidget>
#include <qdebug.h>
class Thumbnail : public QWidget
{
    Q_OBJECT

public:
    explicit Thumbnail(QWidget *parent = nullptr);
};

#endif // THUMBNAIL_H
