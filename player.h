#pragma once

#define __STDC_CONSTANT_MACROS
#define SDL_MAIN_HANDLED

extern "C"
{
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avstring.h"
#include "libswresample/swresample.h"
#include "libavutil/time.h"
#include "libavutil/imgutils.h"

#include "SDL2/SDL.h"
}

#define MAX_AUDIO_BUF_SIZE 19200
#define MAX_VIDEO_PACKET_SIZE (1024 * 1024)
#define MAX_AUDIO_PACKET_SIZE (1024 * 64)
#define PICTURE_QUEUE_SIZE 2
#define VIDEO_REFRESH_EVENT (SDL_USEREVENT)
#define VIDEO_QUIT_EVENT (SDL_USEREVENT + 1)

//ѹ�����ݰ�����
struct PacketQueue
{
    int size;//�������ݰ�����
    AVPacketList *head, *tail;//��βָ��
    SDL_mutex *qMutex;//���л���������ֹͬʱ���ʹ����ڴ�ռ�
    SDL_cond *qCond;//�����ź���������ͬ��
};

//��Ƶ���ͼ�����ݽṹ
struct VideoPicture
{
    uint8_t *buffer;//ΪframeYUV�ṩ����ռ�
    AVFrame *frameYUV;//һ֡������ɵ�YUV��ʽͼ��
    int allocated, width, height;//�ÿռ����ı�־�Լ�ͼ��Ŀ�͸�
    double pts;//��֡ͼ�񲥷ŵ�ʱ���
};

//��¼�򿪵���Ƶ�ļ���̬��Ϣ
struct VideoInf
{
    //��Ƶ�ļ���ȫ�ֲ���
    int quit;//�ر���Ƶ�ı�־
    char file_name[1024];//��Ƶ�ļ���
    AVFormatContext *avFormatCtx;//��Ƶ�ļ�������
    int video_idx, audio_idx;//����Ƶ���±�
    AVCodecContext *vCodecCtx, *aCodecCtx;//����Ƶ������������
    AVCodec *vCodec, *aCodec;//����Ƶ������
    AVStream *vStream, *aStream;//����Ƶ��
    SDL_Thread *parse_tid, *decode_tid;//��Ƶ�ļ������߳�id����Ƶ�������߳�id
    int volume;//������С����ΧΪ[0, SDL_MIX_MAXVOLUME]
    int fullScreen;//ȫ��״̬
    double speed;//�����ٶ�
    int pause;//�Ƿ���ͣ
    int seeking;//��ת���Ŵ����еı�־
    int seek_flag;//���/���˵ı�־
    double tar_pts;//��ת���ŵ�Ŀ��ʱ���
    SDL_mutex *screen_mutex;//��Ļ�������ڸı���Ļ��С�ȹ���
    SDL_Window *screen;//��Ƶ���Ŵ���
    SDL_Renderer *renderer;//��Ƶ������Ⱦ��
    SDL_Texture *texture;//ͼƬ����

    //��Ƶ�������
    VideoPicture picture_queue[PICTURE_QUEUE_SIZE];//ͼ�񻺴���У���Ž��벢ת������ڲ��ŵ�ͼ������
    int picq_ridx, picq_widx, picq_size;//ͼ�񻺴���еĶ�д�±��Լ����еĴ�С
    SDL_mutex *picq_mutex;//ͼ�񻺴���л�����
    SDL_cond *picq_cond_write;//ͼ�񻺴����дͬ���ź���
    PacketQueue video_queue;//��Ƶѹ�����ݰ�����
    int width, height;//��Ƶͼ��Ŀ�͸�
    SwsContext *swsCtx;//��Ƶ��ʽת��������
    double video_pts;//��¼������Ƶ֡��ʱ���
    double video_time;//��Ҫ������Ƶ֡�Ĳ���ʱ�䣬��ϵͳʱ��Ϊ��׼
    double pre_vpts, pre_vdelay;//��¼��һ����Ƶ֡��ʱ������ӳ�

    //��Ƶ�������
    PacketQueue audio_queue;//��Ƶѹ�����ݰ�����
    AVPacket aPacket;//���ڽ������Ƶ���ݰ�
    Uint8 *audio_buf;//���ڲ��ŵ���Ƶ����
    int audio_buf_idx, audio_buf_size;//�Ѿ����벥��������Ľ����������͸�֡�ܵ�������
    SwrContext *swrCtx;//��Ƶ�ز���������
    double audio_pts;//��ǰ������Ƶ֡��ʱ������������ʱ��������ʱ������
};

//��̨�������࣬����ʵ����Ƶ���ŵĹ���
class Player
{
public:
    Player();//���캯��
    ~Player();//��������
    void init();//��ʼ����
    bool playing();//�Ƿ����ڲ�����Ƶ��
    void play(const char input_file[]);//������Ƶ

private:
    VideoInf *av;

};
