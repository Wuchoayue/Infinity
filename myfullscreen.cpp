#include "myfullscreen.h"

myfullscreen::myfullscreen(QWidget *parent)
    : QWidget{parent}
{
    resize(530,480);
    vw = new QWidget(this);
    pc = new PlayerControls(this);
    pc->setWindowFlags(Qt::WindowStaysOnTopHint);
//    pc->setStyleSheet("QWidget{background-color:rgba(0,0,0,100%)}");
//    pc->setWindowOpacity(0.7);
    vw->resize(size());
    setPosition();
    pc->setVisible(isPcVisable);
//    pc->raise();
    pc->show();
    tm = new QTimer();
    tm->setInterval(200);

    connect(tm, &QTimer::timeout, this, &myfullscreen::turnToInvisable);
}

//初始化函数
void myfullscreen::init(){
    qDebug()<<size();
}
//设置位置
void myfullscreen::setPosition(){
    pc->setGeometry(0,height()-50,width(),50);
    vw->setGeometry(0,0,width(),height());
}
//转为不可见
void myfullscreen::turnToInvisable(){
//        if(!pc->geometry().contains(this->mapFromGlobal(QCursor::pos()))){
//            pcLifeTime--;
//            setCursor(Qt::ArrowCursor);
//        }
//        else{
//            isPcVisable=true;
//            pc->setVisible(isPcVisable);
//            pcLifeTime=10;
//        }
//        if(pcLifeTime==0){
//            setCursor(QCursor(Qt::BlankCursor));
//            isPcVisable=false;
//            pc->setVisible(isPcVisable);
//            pcLifeTime=10;
//        }
    if(pcLifeTime>0){
        if(!pc->geometry().contains(this->mapFromGlobal(QCursor::pos()))){
        pcLifeTime--;
        }
        setCursor(Qt::ArrowCursor);
        isPcVisable=true;
    }
    else{
        setCursor(QCursor(Qt::BlankCursor));
        isPcVisable=false;
    }
    pc->setVisible(isPcVisable);
}
//变为全屏
void myfullscreen::turnToFullScreen(){
//    mysize = size();
//    resize(1, 1);
//    showMaximized();
//    setFullScreen(true);

//    pc->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);

    tm->start();
    pcLifeTime=10;
    setWindowFlags(Qt::Window);
    showFullScreen();
    setPosition();
    isPcVisable=true;
    pc->setVisible(isPcVisable);
    vw->resize(size());
    setMouseTracking(true);
    vw->setMouseTracking(true);
    pc->setMouseTracking(true);
}

//退出全屏
void myfullscreen::turnToNormal(){

//    pc->setWindowFlags(Qt::SubWindow|Qt::WindowStaysOnTopHint);
//    pc->setParent(this);
//    setMouseTracking(false);
    pc->show();

    tm->stop();
//    isPcVisable=false;
//    pc->setVisible(isPcVisable);
    setWindowFlags(Qt::SubWindow);
    showNormal();
//    pc->raise();
    setPosition();
    vw->resize(size());
//    isPcVisable=true;
//    pc->setVisible(isPcVisable);
//    setFullScreen(false);
//    resize(mysize);
    setMouseTracking(false);
    vw->setMouseTracking(false);
    pc->setMouseTracking(false);
    setCursor(Qt::ArrowCursor);
}

void myfullscreen::resizeEvent(QResizeEvent* event) {
    this->QWidget::resizeEvent(event);
    this->setPosition();
    qDebug()<<parentWidget()->y();
    qDebug()<<this->height();
    this->pc->duration_slider->thumbnail_y=parentWidget()->y()+height();
}
void myfullscreen::mouseMoveEvent(QMouseEvent *e)
{
    this->QWidget::mouseMoveEvent(e);
    pcLifeTime=10;
}
