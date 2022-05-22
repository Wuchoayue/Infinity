#include "myfullscreen.h"

myfullscreen::myfullscreen(QWidget *parent)
    : QWidget{parent}
{
    resize(530,480);
//        setWindowFlags(Qt::Window);
//    showFullScreen();
//    setWindowFlags(Qt::SubWindow);
//    showNormal();
    vw = new MyVideoWidget(this);

    pc = new PlayerControls(this);
    pc->setWindowFlags(Qt::WindowStaysOnTopHint);
//    pc->setStyleSheet("QWidget{background-color:rgba(0,0,0,100%)}");
    setStyleSheet(video_widget_style);
//    pc->setWindowOpacity(0.7);
    vw->resize(size());
    setPosition();
    pc->setVisible(isPcVisable);
    setMouseTracking(true);
    vw->setMouseTracking(true);
    pc->setMouseTracking(true);
//    pc->raise();
    pc->show();
    tm = new QTimer();
    tm->setInterval(200);
    tm->start();
    connect(tm, &QTimer::timeout, this, &myfullscreen::turnToInvisable);
}

//初始化函数
void myfullscreen::init(){
    qDebug()<<size();
}
//设置位置
void myfullscreen::setPosition(){
    pc->setGeometry(0,height()-70,width(),70);
    vw->setGeometry(0,0,width(),height());
}
//转为不可见
void myfullscreen::turnToInvisable(){

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


    pc->show();

//    tm->stop();
//    setMouseTracking(false);
//    vw->setMouseTracking(false);
//    pc->setMouseTracking(false);


    setWindowFlags(Qt::SubWindow);
    showNormal();

    setPosition();
    vw->resize(size());
    setCursor(Qt::ArrowCursor);
}

void myfullscreen::resizeEvent(QResizeEvent* event) {
    this->QWidget::resizeEvent(event);
    this->setPosition();

}
void myfullscreen::mouseMoveEvent(QMouseEvent *e)
{
    this->QWidget::mouseMoveEvent(e);
    pcLifeTime=10;
//    this->pc->duration_slider->thumbnail_y=parentWidget()->y()+height();
}
