#include "infinityplayer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InfinityPlayer w;
    w.show();
    w.getPlayerControls()->getVolumeControl()->move(388, 315);
    return a.exec();
}
