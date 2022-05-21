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

#include <QString>

#define MAX_AUDIO_BUF_SIZE 19200
#define MAX_VIDEO_PACKET_SIZE (1024 * 1024)
#define MAX_AUDIO_PACKET_SIZE (1024 * 64)
#define MAX_WID_SIZE 100
#define PICTURE_QUEUE_SIZE 2
#define INIT_DTS_VALUE -1000000
#define VIDEO_REFRESH_EVENT (SDL_USEREVENT)
#define VIDEO_QUIT_EVENT (SDL_USEREVENT + 1)

struct VideoInfo{
    QString name;   //�ļ���
    QString type;   //�ļ�����
    QString path;   //�ļ�·��
    QString size;   //�ļ���С
    QString duration;   //ʱ��
    QString v_bit_rate;   //��Ƶ����
    QString frame_rate; //֡��
    QString resolving;  //�ֱ���
    QString a_bit_rate;   //��Ƶ����
    QString sample_rate; //������
    QString channels;   //������
};

struct AudioInfo {
    QString name;   //�ļ���
    QString type;   //�ļ�����
    QString path;   //�ļ�·��
    QString size;   //�ļ���С
    QString duration;   //ʱ��
    QString bit_rate;   //��Ƶ����
    QString sample_rate; //������
    QString channels;   //������
    QString album; //ר����Ϣ
    QString singer;  //�ݳ�����Ϣ
};


//ѹ�����ݰ����нṹ��
struct PacketList
{
    AVPacket pkt;
    struct PacketList *next;
};

//ѹ�����ݰ�����
struct PacketQueue
{
    int size;//�������ݰ�����
    PacketList *head, *tail;//��βָ��
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
struct AVInfo
{
    //��Ƶ�ļ���ȫ�ֲ���
    int quit;//�ر���Ƶ�ı�־
    int resized;//���ڳߴ�ı�ı�־
    char file_name[1024];//��Ƶ�ļ���
    void *wid;//��Ƶ����id
    AVFormatContext *avFormatCtx;//��Ƶ�ļ�������
    int video_idx, audio_idx;//����Ƶ���±�
    AVCodecContext *vCodecCtx, *aCodecCtx;//����Ƶ������������
    AVCodec *vCodec, *aCodec;//����Ƶ������
    AVStream *vStream, *aStream;//����Ƶ��
    SDL_Thread *parse_tid, *decode_video_tid, *decode_audio_tid, *refresh_tid;//��Ƶ�ļ������̡߳�����Ƶ�������߳�id����Ƶˢ���߳�id
    int volume;//������С����ΧΪ[0, SDL_MIX_MAXVOLUME]
    int fullScreen;//ȫ��״̬
    double speed;//�����ٶ�
    int pause;//�Ƿ���ͣ
    int seeking;//��ת���Ŵ����еı�־
	int getFrameFlag;//��ȡ��������ĳ֡��������ı�־
	double getFramePts;//��ȡ��֡����λ�ã���λΪ��
    double tar_pts;//��ת���ŵ�Ŀ��ʱ���
	int64_t last_vdts, last_adts;//�洢�����߳������һ���õ�������Ƶ����dts
    SDL_mutex *screen_mutex;//��Ļ�������ڸı���Ļ��С�ȹ���
    SDL_Window *screen;//��Ƶ���Ŵ���
    SDL_Renderer *renderer;//��Ƶ������Ⱦ��
    SDL_Texture *texture;//ͼƬ����

    //��Ƶ�������
    VideoPicture picture_queue[PICTURE_QUEUE_SIZE];//ͼ�񻺴���У���Ž��벢ת������ڲ��ŵ�ͼ������
    int picq_ridx, picq_widx, picq_size;//ͼ�񻺴���еĶ�д�±��Լ����еĴ�С
    SDL_mutex *picq_mutex;//ͼ�񻺴���л�����
    SDL_cond *picq_cond_read, *picq_cond_write;//ͼ�񻺴���ж�дͬ���ź���
    SDL_mutex *refresh_mutex;//������Ƶˢ���̵߳Ļ�����
    SDL_cond *refresh_cond;//���ڿ���ˢ����һ֡��ͬ���ź���
    PacketQueue video_queue;//��Ƶѹ�����ݰ�����
    int width, height;//��Ƶͼ��Ŀ�͸�
    SwsContext *swsCtx;//��Ƶ��ʽת��������
    double video_pts;//��¼������Ƶ֡��ʱ���
    double video_time;//��Ҫ������Ƶ֡�Ĳ���ʱ�䣬��ϵͳʱ��Ϊ��׼
    double pre_vpts, pre_vdelay;//��¼��һ����Ƶ֡��ʱ������ӳ�

    //��Ƶ�������
    PacketQueue audio_queue;//��Ƶѹ�����ݰ�����
    Uint8 *audio_buf;//���ڲ��ŵ���Ƶ���ݻ���
    int audio_buf_idx, audio_buf_size;//�Ѿ����벥��������Ľ����������͸�֡�ܵ�������
	SDL_mutex *abuf_mutex;//��Ƶ���ݻ���Ļ�����
	SDL_cond *abuf_cond_read, *abuf_cond_write;//��Ƶ���ݻ���Ķ�дͬ���ź���
	Uint8 *audio_buf_test;//����ͼ����
	int audio_buf_size_test;//����ͼ���ݵĳ���
	SDL_mutex *abuf_mutex_test;//����abuf_test�Ļ�����
	SDL_cond *cond_test;//���ڿ��Ʋ����߳̽��ж�ȡ���ź���
	SDL_cond *abuf_cond_read_test, *abuf_cond_write_test;//����abuf_test�Ķ�дͬ���ź���
	SDL_Thread *test_tid;//���ڻ�ȡ����ͼ���ݵĺ�̨�����߳�
    SwrContext *swrCtx;//��Ƶ�ز���������
    double audio_pts;//��ǰ������Ƶ֡��ʱ������������ʱ��������ʱ������
};

//��̨�������࣬����ʵ����Ƶ���ŵĹ���
class Player
{
public:
    Player();//���캯��
    ~Player();//��������

public:
    //���¹�����������Ϊǰ̨���õĹ��ܺ��������жϲ����ĺϷ��ԣ�������ü���
    bool Playing();//�Ƿ������Ƶ�ļ�
    void Play(const char input_file[], void *wid = NULL);//������Ƶ��������Ƶ�ļ�·���ʹ��ڿؼ���winID
    int Pausing();//�ж��Ƿ�����ͣ״̬������1��ʾ��ͣ�У�����0��ʾ�����У�����-1��ʾû�д��κ���Ƶ
    void Pause();//��ͣ-�����л�����
    double GetTotalDuration();//��ȡ��Ƶ���ܳ��ȣ�û�д��κ���Ƶ�ͷ���-1����λ10ms
    double MyGetCurrentTime();//��ȡ��ǰ���Ž��ȣ�û�д��κ���Ƶ�ͷ���-1, ��λ10ms
	bool GetFrameJpg(double pts);//��ȡָ��ʱ�̵�֡���棬��λ10ms
    bool Jump(double play_time);//��ת���ţ�������ת����ʱ��㣬��λ10ms�������Ƿ���ת�ɹ�
    void Backward(double t);//���ˣ�ÿ����תt��
    void Forward(double t);//�����ÿ����תt��
    bool SetSpeed(double speed);//���ò����ٶȣ���ʱ����[0.5, 8]�������Ƿ����óɹ�
    void SpeedUp();//�ӿ첥���ٶȣ�ÿ���ٶ�����0.5����Χ[0.5, 8]
    void SpeedDown();//���Ͳ����ٶȣ�ÿ���ٶȽ���0.5����Χ[0.5, 8]
    bool SetVolume(int volume);//��������������ֵ��Χ[0, 100]�������Ƿ����óɹ�
    void VolumeUp();//���������ÿ������2����
    void VolumeDown();//����������ÿ�ν���2����
    void FullScreen();//ȫ����ÿ����һ�ζ���ı�ȫ��״̬������ȫ��-����֮��ת����
    void Quit();//�˳������������رյ�ǰ��Ƶ�ļ�
	int GetAudioBuf(Uint8 **audio_buf);//��ȡ��ǰ������Ƶ֡�����ݣ�����*audio_buf�У�������buf��size������-1ʱ��ʾ��ȡʧ��
    bool isVideo(); //�ж��ǲ�����Ƶ

private:
    void Init();//��ʼ����
	AVInfo *av;

};

//�����ļ�������Ƶ������Ϣ������vi��ai�ṹ����
//��ȡ��Ƶ�ļ��ķ�����Ϣ(�����Ƶ�ĵ�һ֡��JPG��ʽ)
//������Ƶ·��file_path����ŷ����·��url
//�Ż�true��ʾ��ȡ����ɹ�����������url��
bool DrawJPG(const char file_path[], const char url[]);

//��ȡ�ļ�����Ƶ��������Ϣ�����ػ�ȡ�ɹ����
bool GetInfo(const char file_path[], VideoInfo *vi, AudioInfo *ai);
