#include "infinityplayer.h"

#include <QApplication>
#include <QSharedMemory>
#include "player.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory shared_memory;
    shared_memory.setKey(QString("infinity"));//设置固定共享内存段的key值
    if(shared_memory.attach())   //尝试将进程附加到该共享内存段
    {
       return 0;
    }

    if(shared_memory.create(1)) //创建1byte的共享内存段
    {
        InfinityPlayer w;
        w.show();
        w.getPlayerControls()->getVolumeControl()->move(388-18, 315);
        w.getVideo()->getWaveform()->resize(w.getVideo()->size().width(), 410);
        w.move(600,300);
        return a.exec();
    }

}
