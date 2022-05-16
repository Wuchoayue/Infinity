#include "myfullscreen.h"

myfullscreen::myfullscreen(QWidget *parent)
    : QWidget{parent}
{
    //加载qss样式文件
    QFile file(":/qss/myFullScreen.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    setStyleSheet(stylesheet);
    file.close();
    resize(530, 480);
    vw = new QWidget(this);
    vw->setObjectName("vw");
    pc = new PlayerControls(this);
    pc->setWindowFlags(Qt::WindowStaysOnTopHint);

    vw->resize(size());
    setPosition();
    setMouseTracking(true);
    vw->setMouseTracking(true);
    pc->setVisible(isPcVisable);
    pc->show();

    tm = new QTimer();
    tm->setInterval(200);
    tm->start();
    connect(tm, &QTimer::timeout, this, &myfullscreen::turnToInvisable);
    vw->setUpdatesEnabled(false);
}

//设置位置
void myfullscreen::setPosition() {
    isPcVisable = true;
    pc->setVisible(isPcVisable);
    pc->setGeometry(0,height()-70,width(),70);
    vw->setGeometry(0,0,width(),height());
    pc->getVolumeControl()->move(pc->getVolumeShow_button()->x() - 12, height() - 165);
}

//转为不可见
void myfullscreen::turnToInvisable() {
    if(pcLifeTime>0) {
        if(!pc->getVolumeControl()->isVisible() && !pc->geometry().contains(this->mapFromGlobal(QCursor::pos()))){
            pcLifeTime--;
        }
        setCursor(Qt::ArrowCursor);
        isPcVisable = true;
    }
    else{
        setCursor(QCursor(Qt::BlankCursor));
        isPcVisable = false;
    }
    pc->setVisible(isPcVisable);
    pc->getVolumeControl()->setVisible(pc->getVolumeControl()->isVisible() && isPcVisable);
}

PlayerControls *myfullscreen::getPc() const
{
    return pc;
}

QWidget *myfullscreen::getVw() const
{
    return vw;
}

void myfullscreen::turnToFullScreen() {
    pcLifeTime=10;
    setWindowFlags(Qt::Window);
    showFullScreen();
    setPosition();
    isPcVisable = true;
    pc->setVisible(isPcVisable);
    vw->resize(size());
    //    setMouseTracking(true);
    //    vw->setMouseTracking(true);
    pc->setMouseTracking(true);
    pc->getChangeMediaDirShow_button()->setVisible(false);
    pc->getShowList_button()->setVisible(false);
}

void myfullscreen::turnToNormal() {
    pc->show();
    setWindowFlags(Qt::SubWindow);
    showNormal();
    setPosition();
    vw->resize(size());
    setCursor(Qt::ArrowCursor);
    pc->getChangeMediaDirShow_button()->setVisible(true);
    pc->getShowList_button()->setVisible(true);
}

void myfullscreen::resizeEvent(QResizeEvent* event) {
    this->QWidget::resizeEvent(event);
    this->setPosition();
}

void myfullscreen::mouseMoveEvent(QMouseEvent *e)
{
    this->QWidget::mouseMoveEvent(e);
    pcLifeTime = 10;
}

