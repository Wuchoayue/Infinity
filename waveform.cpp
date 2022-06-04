#include "waveform.h"
#include <QDebug>
#include <QCoreApplication>




Waveform::Waveform(QWidget *parent)
    : QWidget{parent}
{
    //this->setFixedSize(720,220);//固定大小
//    switchButton = new QPushButton(this);
//    switchButton->setFixedSize(15,15);
//    switchButton->move(5,5);
//    connect(switchButton,&QPushButton::clicked,this,[=]{
//        if(style=="a")
//            setStyle("b");
//        else
//            setStyle("a");
//    });

    buf = nullptr;
    bufSize = -1;
    numSamples = 80;//采样点数量
    delay = 50;//50ms绘制一次
    samples = new double[numSamples];
    //初始化
    for(int i=0;i<numSamples;++i)
    {
        samples[i]=0;
    }

    style = "b";
    paintFlag = false;
    QTimer::singleShot(delay, this,SLOT(onTimer()));
    filepath=":/icon/background.png";
}

Waveform::~Waveform()
{
    delete[] samples;
}

void Waveform::paintEvent(QPaintEvent *e)
{
    if(paintFlag)
    {
        QPainter painter(this);

        bufSize = player->GetAudioBuf(&buf);// 获取缓冲区地址
        short* data = (short*)buf;
        if (bufSize !=-1 && bufSize/4>numSamples)//返回的pcm点大于采样点
        {
            int delta = (bufSize/numSamples)/4;
            int base = 0;
            double avg;
            // 进行采样
            for(int i=0;i<numSamples;++i)
            {
                //取平均值
                avg=0;
                for(int j=0;j<delta;++j)
                {
                    //对数据进行归一化(-1,1)
                    avg += data[base++]/32767.0;

                }
                avg /= delta;


                samples[i]= avg;
            }

            //绘制
            double vx = 20;//左右间距
            double ix = 3;//矩形间的间距
            double dx = (width()-vx*2-ix*(numSamples-1))/numSamples;//求宽度
            //qDebug()<<width();
            //qDebug()<<vx+(numSamples-1)*(dx+ix)+vx;

            double dh,vh=20;//vh是距离底部的像素
            painter.save();
            //第一种样式
            if(style=="a")
            {
                for(int i=0;i<numSamples;++i)
                {
                    dh = height()*(1+samples[i])/2;
                    painter.drawRect(vx+i*(dx+ix),height()-vh-dh,dx,dh);//画矩形

                }
            }
            else if(style=="b")
            {
                for(int i=0;i<numSamples;++i)
                {
                    dh = height()*(samples[i]);
                    painter.drawRect(vx+i*(dx+ix),height()/2,dx,dh);//画矩形

                }
            }
            painter.restore();
        }
    }
    else
    {

      QPainter painter(this);
      painter.save();
      painter.drawPixmap(rect(),QPixmap(filepath));
      painter.restore();
    }




}

void Waveform::setPlayer(Player*p){
    player=p;
}

void Waveform::onTimer()
{
    // 一直开
    update();
    //if(paintFlag)
    QTimer::singleShot(delay, this,SLOT(onTimer()));

}

void Waveform::setPath(QString path)
{
    filepath = path;
}

void Waveform::control()
{
    paintFlag = !paintFlag;
}

void Waveform::setStyle(QString str)
{
    style = str;
}

void Waveform::closeEvent(QCloseEvent *event)
{
    emit closeSignal();
}
