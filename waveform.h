#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QPushButton>
#include "player.h"

class Waveform : public QWidget
{
    Q_OBJECT
public:
    explicit Waveform(QWidget *parent = nullptr);
    ~Waveform();
    void control();//控制开始绘图和暂停绘图
    void setStyle(QString);

    void setPath(QString);
    void setPlayer(Player* );

private:
    void closeEvent(QCloseEvent *event);//关闭事件
    void paintEvent(QPaintEvent *);//绘图事件
    Uint8*buf;
    short*data;
    int bufSize;
    int numSamples;
    bool paintFlag;
    double * samples;
    int delay;//定时器延迟
    Player *player;
    QTimer timer;
    QString style;
    QPushButton* switchButton;
    QString filepath;


signals:
    void closeSignal();

private slots:
    void onTimer();
    void loadData();

};

#endif // WAVEFORM_H
