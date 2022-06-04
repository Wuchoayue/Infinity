#include "mediainfo.h"

MediaInfo::MediaInfo(QWidget *parent)
    : QWidget(parent)
{
    //表格
    info = new QTableWidget(this);
    info->setSelectionBehavior(QAbstractItemView::SelectRows);
    //表头
    QStringList headers;
    headers << "属性" << "值";
    info->setColumnCount(2);
    info->setHorizontalHeaderLabels(headers);
    setFixedSize(400, 450);
    setStyleSheet("QTableWidget{margin: 15px; gridline-color: transparent;} QTableWidget::item {border: none; padding-left: 3px} QHeaderView{gridline-color: transparent; "
                  "font-size: 13px; font-weight: 600} QTableView::item:selected {background-color: rgb(196, 255, 220); color: #000000}");
    info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    info->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    info->setColumnWidth(0, 100);
    info->resize(size());
    info->setFocusPolicy(Qt::NoFocus);
    info->verticalHeader()->setHidden(true);
}

MediaInfo::~MediaInfo()
{
    qDebug() << "over";
}

void MediaInfo::setInfo(VideoInfo videoInfo)
{
    info->setRowCount(11);

    //基础信息
    info->setItem(0, 0, new QTableWidgetItem("名称"));
    info->setItem(1, 0, new QTableWidgetItem("类型"));
    info->setItem(2, 0, new QTableWidgetItem("文件路径"));
    info->setItem(3, 0, new QTableWidgetItem("大小"));
    info->setItem(4, 0, new QTableWidgetItem("时长"));

    //高级信息
    info->setItem(5, 0, new QTableWidgetItem("视频码率"));
    info->setItem(6, 0, new QTableWidgetItem("帧率"));
    info->setItem(7, 0, new QTableWidgetItem("分辨率"));
    info->setItem(8, 0, new QTableWidgetItem("音频码率"));
    info->setItem(9, 0, new QTableWidgetItem("采样率"));
    info->setItem(10, 0, new QTableWidgetItem("声道数"));

    //设置信息
    info->setItem(0, 1, new QTableWidgetItem(videoInfo.name));
    info->setItem(1, 1, new QTableWidgetItem(videoInfo.type));
    info->setItem(2, 1, new QTableWidgetItem(videoInfo.path));
    info->setItem(3, 1, new QTableWidgetItem(videoInfo.size));
    info->setItem(4, 1, new QTableWidgetItem(videoInfo.duration));
    info->setItem(5, 1, new QTableWidgetItem(videoInfo.v_bit_rate));
    info->setItem(6, 1, new QTableWidgetItem(videoInfo.frame_rate));
    info->setItem(7, 1, new QTableWidgetItem(videoInfo.resolving));
    info->setItem(8, 1, new QTableWidgetItem(videoInfo.a_bit_rate));
    info->setItem(9, 1, new QTableWidgetItem(videoInfo.sample_rate));
    info->setItem(10, 1, new QTableWidgetItem(videoInfo.channels));

    show();
}

void MediaInfo::setInfo(AudioInfo audioInfo)
{
    info->setRowCount(10);

    //基础信息
    info->setItem(0, 0, new QTableWidgetItem("名称"));
    info->setItem(1, 0, new QTableWidgetItem("类型"));
    info->setItem(2, 0, new QTableWidgetItem("文件路径"));
    info->setItem(3, 0, new QTableWidgetItem("大小"));
    info->setItem(4, 0, new QTableWidgetItem("时长"));

    //高级信息
    info->setItem(5, 0, new QTableWidgetItem("音频码率"));
    info->setItem(6, 0, new QTableWidgetItem("采样率"));
    info->setItem(7, 0, new QTableWidgetItem("声道数"));
    info->setItem(8, 0, new QTableWidgetItem("专辑"));
    info->setItem(9, 0, new QTableWidgetItem("演唱者"));

    //设置信息
    info->setItem(0, 1, new QTableWidgetItem(audioInfo.name));
    info->setItem(1, 1, new QTableWidgetItem(audioInfo.type));
    info->setItem(2, 1, new QTableWidgetItem(audioInfo.path));
    info->setItem(3, 1, new QTableWidgetItem(audioInfo.size));
    info->setItem(4, 1, new QTableWidgetItem(audioInfo.duration));
    info->setItem(5, 1, new QTableWidgetItem(audioInfo.bit_rate));
    info->setItem(6, 1, new QTableWidgetItem(audioInfo.sample_rate));
    info->setItem(7, 1, new QTableWidgetItem(audioInfo.channels));
    info->setItem(8, 1, new QTableWidgetItem(audioInfo.album));
    info->setItem(9, 1, new QTableWidgetItem(audioInfo.singer));

    show();
}
