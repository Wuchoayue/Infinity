#include <stdio.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <sstream>
#include <random>
#include <QDebug>
#include <QTime>
#include "player.h"
#include "sonic.h"

using namespace std;

sonicStream sncStream;//PCM数据倍速处理流
AVPacket flush_pkt;//包标志，标志跳转播放模式（快进/快退等）

//创建压缩数据包队列
void CreatePacketQueue(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    q->qMutex = SDL_CreateMutex();
    q->qCond = SDL_CreateCond();
}

//清空压缩数据包队列
void ClearPacketQueue(PacketQueue *q)
{
    SDL_LockMutex(q->qMutex);//给共享内存区上锁
    PacketList *pktList;
    int cnt = 0;
    while (q->head && q->head->next)
    {
        pktList = q->head;
        q->head = q->head->next;
        if (pktList->pkt.data)
        {
            av_packet_unref(&pktList->pkt);
        }
        if(pktList) av_free(pktList);
    }
    if(q->head && q->head->pkt.data)
    {
        av_free(q->head);
    }
    q->size = 0;
    q->tail = q->head = NULL;
    SDL_UnlockMutex(q->qMutex);
}

//销毁压缩数据包队列，释放空间
void DestroyPacketQueue(PacketQueue *q)
{
    ClearPacketQueue(q);
    SDL_DestroyMutex(q->qMutex);
    SDL_DestroyCond(q->qCond);
    q->qMutex = NULL;
    q->qCond = NULL;
}

//将压缩数据包pkt进队
void PacketQueuePut(PacketQueue *q, AVPacket *pkt)
{
    PacketList *pktList = (PacketList *)av_mallocz(sizeof(PacketList));
    pktList->pkt = *pkt;
    pktList->next = NULL;

    //访问共享内存区
    SDL_LockMutex(q->qMutex);
    if (q->size == 0)//队列中无数据
    {
        q->head = pktList;
        q->tail = pktList;
    }
    else
    {
        q->tail->next = pktList;
        q->tail = pktList;
    }
    q->size++;
    SDL_CondSignal(q->qCond);
    SDL_UnlockMutex(q->qMutex);
}

//从数据包队列中取出数据放到pkt中，成功放回0
int PacketQueueGet(AVInfo *av, PacketQueue *q, AVPacket *pkt)
{
    PacketList *pktList;
    //访问共享内存区
    SDL_LockMutex(q->qMutex);
    while (q->size == 0)//队列中无数据
    {
        SDL_CondWait(q->qCond, q->qMutex);//阻塞等待，并暂时释放互斥锁
        if (av->quit)
        {
            printf("user shuts the video!\n");
            SDL_UnlockMutex(q->qMutex);
            return -1;
        }
    }
    if (q->head == NULL)
    {
        printf("packet queue is empty, failed!\n");
        return -1;
    }
    pktList = q->head;
    *pkt = q->head->pkt;
    q->head = q->head->next;
    q->size--;
    if (q->size == 0)
    {
        q->tail = NULL;
    }
    av_free(pktList);
    SDL_UnlockMutex(q->qMutex);

    return 0;
}

//清空图像缓存队列
void ClearPictureQueue(AVInfo *av)
{
    SDL_LockMutex(av->picq_mutex);
    while (av->picq_size)
    {
        av->picq_ridx++;
        if (av->picq_ridx >= PICTURE_QUEUE_SIZE)
        {
            av->picq_ridx = 0;
        }
        av->picq_size--;
    }
    SDL_CondSignal(av->picq_cond_write);
    SDL_UnlockMutex(av->picq_mutex);
}

//清空音频缓存
void ClearAudioBuf(AVInfo *av)
{
    SDL_LockMutex(av->abuf_mutex);
    av->audio_buf_size = 0;
    SDL_CondSignal(av->abuf_cond_write);
    SDL_UnlockMutex(av->abuf_mutex);
}

//分配图像缓存空间，已经正确分配就不做任何操作
void AllocatePicture(AVInfo *av, VideoPicture *vp)
{
    //该空间缓存是否未分配或者图像的宽高不正确，需要重新分配空间
    if (!vp->allocated || vp->width != av->width || vp->height != av->height)
    {
        if (vp->buffer)//确保没有分配空间，防止内存泄漏
        {
            av_free(vp->buffer);
        }
        if (vp->frameYUV)//确保没有分配空间，防止内存泄漏
        {
            av_frame_free(&vp->frameYUV);
        }

        vp->allocated = 1;
        vp->width = av->width;
        vp->height = av->height;

        //分配空间并给frameYUV挂上buffer缓存
        vp->buffer = (uint8_t *)av_mallocz(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, av->width, av->height, 1));
        vp->frameYUV = av_frame_alloc();
        av_image_fill_arrays(vp->frameYUV->data, vp->frameYUV->linesize, vp->buffer, AV_PIX_FMT_YUV420P, av->width, av->height, 1);
    }
}

//获取实际的音频时间
double GetAudioTime(AVInfo *av)
{
    double pts = av->audio_pts;
    int nb_samples = av->audio_buf_size / av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO) / 2;//缓存帧的采样数
    double frame_time = 1.0 * nb_samples / av->aCodecCtx->sample_rate;//缓存帧的时长
    pts -= frame_time * (av->audio_buf_size - av->audio_buf_idx) / av->audio_buf_size;

    return pts;
}

//音频倍速处理函数
//输入采样点数nb_samples
//将倍速处理后的数据写回av->audio_buf中并返回处理后的nb_samples
int ChangeAudioSpeed(AVInfo *av, int nb_samples)
{
    int nb_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);//立体声

    if (!sncStream)
    {	// 参数为采样率和声道数
        sncStream = sonicCreateStream(av->aCodecCtx->sample_rate, nb_channels);
    }

    // 设置流的速率
    sonicSetSpeed(sncStream, float(av->speed));
    // 向流中写入audio_buf
    int ret = sonicWriteShortToStream(sncStream, (short *)av->audio_buf, nb_samples);
    // 计算处理后的采样点数
    int numSamples = int(nb_samples / av->speed);
    if (ret)
    {	// 从流中读取处理好的数据
        nb_samples = sonicReadShortFromStream(sncStream, (short *)av->audio_buf, numSamples);
    }

    return nb_samples;
}

//解码一帧音频数据，保存到av->audio_buf中
int DecodeAudioThread(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    for (;;)
    {
        if (pkt->data)//pkt中有数据，释放缓存空间，防止内存泄露
        {
            av_packet_unref(pkt);
            pkt->data = NULL;
        }
        if (PacketQueueGet(av, &av->audio_queue, pkt) != 0)//从队列中读取压缩数据包
        {
            printf("get pkt failed!\n");
            if (av->quit)
            {
                printf("user shuts the video!\n");
                av_frame_free(&frame);
                if(pkt->data)
                {
                    av_packet_unref(pkt);
                    av_packet_free(&pkt);
                }
                return 0;
            }
            else
            {
                continue;
            }
        }
        //读取到跳转处理的标志，重置解码器
        if (pkt->data == flush_pkt.data)
        {
            avcodec_flush_buffers(av->aCodecCtx);
            ClearAudioBuf(av);
            continue;
        }

        if (avcodec_send_packet(av->aCodecCtx, pkt) == 0)//将拿到的数据包送到解码器
        {
            if (pkt->pts != AV_NOPTS_VALUE)//更新音频时间戳
            {
                av->audio_pts = pkt->pts * av_q2d(av->aStream->time_base);
            }

            //从解码器中解码得到数据，进行处理后交由音频回调函数送入声卡播放，否则到队列中取包
            while (avcodec_receive_frame(av->aCodecCtx, frame) == 0)
            {
                av->audio_pts += 1.0 * frame->nb_samples / av->aCodecCtx->sample_rate;

                //如果是当前解码数据的播放时间小于av中记录的tar_pts，说明处于跳转过程中，需要忽略关键帧到tar_pts之间的数据
                if (av->audio_pts < av->tar_pts)
                {
                    continue;
                }

                //检查队列是否还有数据未播放
                SDL_LockMutex(av->abuf_mutex);
                while (av->audio_buf_idx < av->audio_buf_size)
                {

                    SDL_CondWait(av->abuf_cond_write, av->abuf_mutex);

                    if (av->quit)
                    {
                        printf("user shuts the video!\n");
                        av_frame_free(&frame);
                        if(pkt->data)
                        {
                            av_packet_unref(pkt);
                            av_packet_free(&pkt);
                        }
                        SDL_UnlockMutex(av->abuf_mutex);
                        return 0;
                    }
                }
                SDL_UnlockMutex(av->abuf_mutex);

                //重采样
                int nb_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
                int nb_samples = swr_convert(av->swrCtx, (uint8_t **)&av->audio_buf, MAX_AUDIO_BUF_SIZE,
                                             (const uint8_t **)frame->data, frame->nb_samples);

                nb_samples = ChangeAudioSpeed(av, nb_samples);

                //进行变速处理，并计算处理完的buf_size，通知音频回调函数可以访问缓存了
                SDL_LockMutex(av->abuf_mutex);
                av->audio_buf_size = av_samples_get_buffer_size(NULL, nb_channels, nb_samples, AV_SAMPLE_FMT_S16, 1);
                if (av->audio_buf_size <= 0)
                {
                    SDL_UnlockMutex(av->abuf_mutex);
                    continue;
                }
                av->audio_buf_idx = 0;
                //SDL_CondSignal(av->cond_waveform);
                SDL_CondSignal(av->abuf_cond_read);
                SDL_UnlockMutex(av->abuf_mutex);
            }
        }
    }
    return -1;
}

//音频回调函数
void AudioCallback(void *user_data, Uint8 *stream, int len)
{
    AVInfo *av = (AVInfo *)user_data;

    SDL_memset(stream, 0, len);

    if(av->quit)
    {
        return;
    }

    while (len > 0)
    {
        //检查音频缓存是否有数据待播放
        SDL_LockMutex(av->abuf_mutex);
        while (av->audio_buf_idx >= av->audio_buf_size)
        {
            SDL_CondWait(av->abuf_cond_read, av->abuf_mutex);
            if (av->quit)
            {
                printf("user shuts the video!\n");
                SDL_UnlockMutex(av->abuf_mutex);
                return;
            }
        }
        SDL_UnlockMutex(av->abuf_mutex);

        //如果处于暂停状态且该帧数据应该播放
        if(av->pause && av->audio_pts >= av->tar_pts && av->seeking != 2)
        {
            return;
        }

        //如果是当前播放时间小于av中记录的tar_pts，说明处于跳转过程中，需要忽略tar_pts之前的数据
        //正在跳转中，尽快处理下一帧
        if (av->seeking == 2)
        {
            SDL_LockMutex(av->abuf_mutex);
            av->audio_buf_idx = av->audio_buf_size;
            SDL_CondSignal(av->abuf_cond_write);
            SDL_UnlockMutex(av->abuf_mutex);
            SDL_Delay(10);
            continue;
        }

        //将解码的数据写入播放器缓存中
        SDL_LockMutex(av->abuf_mutex);
        int write_len = av->audio_buf_size - av->audio_buf_idx;
        if (write_len > len)
        {
            write_len = len;
        }
        SDL_MixAudio(stream, av->audio_buf + av->audio_buf_idx, write_len, av->volume);
        stream += write_len;
        len -= write_len;
        av->audio_buf_idx += write_len;
        SDL_CondSignal(av->abuf_cond_write);
        SDL_UnlockMutex(av->abuf_mutex);
    }
}

//处理视频帧的pts并更新av中的视频时间戳
double synchronize_video(AVInfo *av, AVFrame *src_frame, double pts)
{
    //检查显示时间戳
    if (pts != 0) //检查显示时间戳是否有效
    {
        av->video_pts = pts;//用显示时间戳更新已播放时间
    }
    else //若获取不到显示时间戳
    {
        pts = av->video_pts;//用已播放时间更新显示时间戳
    }
    //更新视频时间戳
    double frame_delay = av_q2d(av->vCodecCtx->time_base);//该帧显示完将要花费的时间
    //若存在重复帧，则在正常播放的前后两帧图像间安排渲染重复帧
    frame_delay += src_frame->repeat_pict*(frame_delay*0.5);//计算渲染重复帧的时值(类似于音符时值)
    av->video_pts += frame_delay;//更新视频播放时间

    return pts;//此时返回的值即为下一帧将要开始显示的时间戳
}

//视频解码线程函数
int DecodeVideoThread(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;
    AVFrame *frame = av_frame_alloc();
    AVPacket *pkt = av_packet_alloc();

    //不断地从视频压缩数据包队列中读取数据包，进行解码，格式转换后放到图像缓存队列中
    for (;;)
    {
        if (pkt->data)//释放缓存的空间，防止内存泄漏
        {
            av_packet_unref(pkt);
            pkt->data = NULL;
        }
        if (PacketQueueGet(av, &av->video_queue, pkt) != 0)
        {
            printf("get pkt failed!\n");
            if (av->quit)
            {
                printf("user shuts the video!\n");
                av_frame_free(&frame);
                if(pkt->data)
                {
                    av_packet_unref(pkt);
                    av_packet_free(&pkt);
                }
                return 0;
            }
            else
            {
                continue;
            }
        }
        //读取到跳转处理的标志，重置解码器
        if (pkt->data == flush_pkt.data)
        {
            ClearPictureQueue(av);
            avcodec_flush_buffers(av->vCodecCtx);
            continue;
        }
        //解码一帧视频压缩数据，得到视频像素数据
        if (avcodec_send_packet(av->vCodecCtx, pkt) == 0)
        {
            //一个pkt中可能有多帧数据，需要循环读取
            while (avcodec_receive_frame(av->vCodecCtx, frame) == 0)
            {
                double pts = frame->pts * av_q2d(av->vStream->time_base);

                //如果是当前播放时间小于av中记录的tar_pts，说明处于跳转过程中，需要忽略关键帧到tar_pts之间的数据
                if (pts < av->tar_pts)
                {
                    continue;
                }

                //检查队列是否已满
                SDL_LockMutex(av->picq_mutex);
                while (av->picq_size >= PICTURE_QUEUE_SIZE)
                {
                    SDL_CondWait(av->picq_cond_write, av->picq_mutex);
                    if (av->quit)
                    {
                        printf("user shuts the video!\n");
                        av_frame_free(&frame);
                        if(pkt->data)
                        {
                            av_packet_unref(pkt);
                            av_packet_free(&pkt);
                        }
                        SDL_UnlockMutex(av->picq_mutex);
                        return 0;
                    }
                }
                SDL_UnlockMutex(av->picq_mutex);

                VideoPicture *vp = &av->picture_queue[av->picq_widx];

                //该空间缓存是否未分配或者图像的宽高不正确，需要重新分配空间
                AllocatePicture(av, vp);

                //获取该帧的时间戳
                vp->pts = synchronize_video(av, frame, pts);

                //对图像进行格式转换
                sws_scale(av->swsCtx, (const uint8_t *const*)frame->data, frame->linesize, 0, av->height,
                          vp->frameYUV->data, vp->frameYUV->linesize);

                //队列长度+1，写位置指针也移动
                SDL_LockMutex(av->picq_mutex);
                av->picq_size++;
                av->picq_widx++;
                if (av->picq_widx >= PICTURE_QUEUE_SIZE)
                {
                    av->picq_widx = 0;
                }
                SDL_CondSignal(av->picq_cond_read);
                SDL_UnlockMutex(av->picq_mutex);
            }
        }
    }

    return 0;
}

//视频刷新定时器回调函数
static Uint32 RefreshTimerCallBack(Uint32 interval, void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;
    SDL_LockMutex(av->refresh_mutex);
    SDL_CondSignal(av->refresh_cond);
    SDL_UnlockMutex(av->refresh_mutex);
    return 0;
}

//视频刷新定时器
void VideoRefreshTimer(AVInfo *av, int delay)
{
    SDL_AddTimer(delay, RefreshTimerCallBack, av);
}

//将vp中的图像刷新到屏幕上
void DisplayPicture(AVInfo *av, VideoPicture *vp)
{
    if (vp->frameYUV)
    {
        //计算图片显示的位置和大小
        SDL_Rect sdl_rect;
        int screen_w, screen_h;
        double ratio = vp->width * 1.0 / vp->height;
        SDL_GetWindowSize(av->screen, &screen_w, &screen_h);
        if(screen_h * ratio > screen_w)
        {
            sdl_rect.x = 0;
            sdl_rect.w = screen_w;
            sdl_rect.h = screen_w / ratio;
            sdl_rect.y = (screen_h - sdl_rect.h) / 2;
        }
        else
        {
            sdl_rect.y = 0;
            sdl_rect.h = screen_h;
            sdl_rect.w = screen_h * ratio;
            sdl_rect.x = (screen_w - sdl_rect.w) / 2;
        }

        //将图片更新到屏幕上
        SDL_LockMutex(av->screen_mutex);
        if(av->resized)//窗口大小发生改变，重新创建纹理和渲染器
        {
            SDL_DestroyTexture(av->texture);
            SDL_DestroyRenderer(av->renderer);
            av->renderer = SDL_CreateRenderer(av->screen, -1, 0);
            av->texture = SDL_CreateTexture(av->renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, av->width, av->height);
            av->resized--;
        }
        SDL_UpdateTexture(av->texture, NULL, vp->frameYUV->data[0], vp->frameYUV->linesize[0]);
        SDL_RenderClear(av->renderer);
        SDL_RenderCopy(av->renderer, av->texture, NULL, &sdl_rect);
        SDL_RenderPresent(av->renderer);
        SDL_UnlockMutex(av->screen_mutex);
    }
}

//视频刷新函数，刷新屏幕并开启下一帧的定时器
int VideoRefresh(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;

    //用户已经关闭播放，停止定时器
    if (av->quit)
    {
        return -1;
    }

    //1.-------刷新显示当前帧到屏幕上---------
    //检查图像缓存队列是否为空
    SDL_LockMutex(av->picq_mutex);
    while (av->picq_size == 0)
    {
        SDL_CondWait(av->picq_cond_read, av->picq_mutex);
        if (av->quit)
        {
            printf("user shuts the video!\n");
            SDL_UnlockMutex(av->picq_mutex);
            return -1;
        }
    }
    SDL_UnlockMutex(av->picq_mutex);

    VideoPicture *vp = &av->picture_queue[av->picq_ridx];

    //检查是否处于暂停状态且该图片需正常显示
    if (av->pause && vp->pts >= av->tar_pts && av->seeking != 2)
    {
        DisplayPicture(av, vp);//将图片显示到屏幕上
        VideoRefreshTimer(av, 10);//10ms后再尝试
        return 0;
    }

    //判断图片是否需要丢弃
    if (vp->pts >= av->tar_pts)
    {
        DisplayPicture(av, vp);//将图片显示到屏幕上
    }
    av->video_time = av_gettime() / 1000000.0;//更新当前帧显示时对应的系统时间

    //改变缓存队列大小和移动读指针
    SDL_LockMutex(av->picq_mutex);
    av->picq_ridx++;
    if (av->picq_ridx >= PICTURE_QUEUE_SIZE)
    {
        av->picq_ridx = 0;
    }
    av->picq_size--;
    SDL_CondSignal(av->picq_cond_write);
    SDL_UnlockMutex(av->picq_mutex);

    //2.-------计算下一帧的显示时间，并启动定时器--------
    double delay = vp->pts - av->pre_vpts;
    if (delay <= 0 || delay >= 1.0)//判断是否合理,否则沿用上一帧的延迟
    {
        delay = av->pre_vdelay;
    }
    av->pre_vdelay = delay;
    av->pre_vpts = vp->pts;

    //当前帧应该忽略，快速处理下一帧
    if (av->seeking == 2)
    {
        delay = 0;
    }

    //判断视频时间戳和音频时间戳的相对快慢，并进行相应调整
    double diff = vp->pts - GetAudioTime(av);
    if (fabs(diff) < 5)
    {
        if (diff < -delay)//视频慢了，快速播放下一帧
        {
            delay = 0;
        }
        else if (diff > delay)//视频快了，延迟播放下一帧
        {
            delay = 2 * delay;
        }
    }
    else //时差过大的话就已经不是同步问题了
    {
        if (diff < 0)//视频慢了，快速播放下一帧
        {
            delay = 0;
        }
        else if (diff > 0)//视频快了，延迟播放下一帧
        {
            delay = 2 * delay;
        }
    }

    //根据调整得到的delay开启定时器
    av->video_time += delay / av->speed;
    double actual_delay = av->video_time - av_gettime() / 1000000.0;
    if (actual_delay < 0.015 / av->speed)
    {
        actual_delay = 0.015 / av->speed;
    }
    VideoRefreshTimer(av, int(actual_delay * 1000 + 0.5));
    return 0;
}

//将播放时间设置跳转到pts
void JumpToPts(AVInfo *av, double pts)
{
    //检验pts的合理性
    if (pts < 0)
    {
        pts = 0;
    }
    else if (pts > av->avFormatCtx->duration / 1000000.0 - 1)
    {
        pts = av->avFormatCtx->duration / 1000000.0 - 1;
    }
    av->tar_pts = pts;
}

//全屏/退出全屏
void MyFullScreen(AVInfo *av)
{
    //还没有生成窗口，退出操作
    if (!av->texture)
    {
        return;
    }

    SDL_LockMutex(av->screen_mutex);
    SDL_DestroyTexture(av->texture);
    SDL_DestroyRenderer(av->renderer);
    SDL_DestroyWindow(av->screen);
    av->fullScreen = !av->fullScreen;
    if (av->fullScreen)
    {
        //创建视频显示窗口
        av->screen = SDL_CreateWindow("MyPlayer", 50, 50, 1280, 720, SDL_WINDOW_OPENGL);
        SDL_ShowWindow(av->screen);
        SDL_SetWindowFullscreen(av->screen, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
    {
        if (av->wid)
        {
            av->screen = SDL_CreateWindowFrom(av->wid);
        }
        else
        {
            av->screen = SDL_CreateWindow("MyPlayer", 50, 50, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        }
        SDL_SetWindowFullscreen(av->screen, SDL_WINDOW_OPENGL);
    }
    av->renderer = SDL_CreateRenderer(av->screen, -1, 0);
    av->texture = SDL_CreateTexture(av->renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, av->width, av->height);
    SDL_UnlockMutex(av->screen_mutex);
}

//关闭视频
void Video_Quit(AVInfo *av)
{
    av->quit = 1;//设置状态

    //发送一下信号量，防止进程阻塞无法退出
    if (av->picq_cond_write)
    {
        SDL_CondSignal(av->picq_cond_write);
    }
    if (av->picq_cond_read)
    {
        SDL_CondSignal(av->picq_cond_read);
    }
    if (av->video_queue.qCond)
    {
        SDL_CondSignal(av->video_queue.qCond);
    }
    if (av->audio_queue.qCond)
    {
        SDL_CondSignal(av->audio_queue.qCond);
    }
    if(av->refresh_cond)
    {
        SDL_CondSignal(av->refresh_cond);
    }
    if (av->abuf_cond_write)
    {
        SDL_CondSignal(av->abuf_cond_write);
    }
    if (av->abuf_cond_read)
    {
        SDL_CondSignal(av->abuf_cond_read);
    }
    if (av->abuf_cond_read_waveform)
    {
        SDL_CondSignal(av->abuf_cond_read_waveform);
    }
    if (av->abuf_cond_write_waveform)
    {
        SDL_CondSignal(av->abuf_cond_write_waveform);
    }
    if (av->cond_waveform)
    {
        SDL_CondSignal(av->cond_waveform);
    }

    //关闭音频播放
    SDL_CloseAudio();

    //关闭视频窗口
    if (av->screen) SDL_HideWindow(av->screen);
}

//视频刷新线程
int VideoRefreshThread(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;

    for (;;)
    {
        SDL_LockMutex(av->refresh_mutex);
        SDL_CondWait(av->refresh_cond, av->refresh_mutex);//等待刷新计时器结束
        if (av->quit)
        {
            printf("user shuts the video!\n");
            SDL_UnlockMutex(av->refresh_mutex);
            return 0;
        }
        if(VideoRefresh(av) < 0)
        {
            if(av->quit)
            {
                printf("user shuts the video!\n");
                SDL_UnlockMutex(av->refresh_mutex);
                return 0;
            }
        }
        SDL_UnlockMutex(av->refresh_mutex);
    }
    return 1;
}

//将frameYUV格式的图片压缩为JPG格式输出到文件url中
int YUV2JPG(AVFrame* avFrame, const char url[])
{
    // 分配AVFormatContext对象
    AVFormatContext* avFormatCtx = avformat_alloc_context();

    // 设置输出文件格式
    avFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
    // 创建并初始化一个和out_file相关的AVIOContext
    if (avio_open(&avFormatCtx->pb, url, AVIO_FLAG_READ_WRITE) < 0)
    {
        avformat_free_context(avFormatCtx);
        return -1;
    }

    // 构建一个新stream
    AVStream* avStream = avformat_new_stream(avFormatCtx, 0);
    if (avStream == NULL)
    {
        avio_close(avFormatCtx->pb);
        avformat_free_context(avFormatCtx);
        return -1;
    }

    //设置对应的解码器
    AVCodec* avCodec = NULL;
    AVCodecContext *avCodecCtx = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecCtx, avStream->codecpar);

    avCodecCtx->codec_id = avFormatCtx->oformat->video_codec;
    avCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    avCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    avCodecCtx->width = avFrame->width;
    avCodecCtx->height = avFrame->height;
    avCodecCtx->time_base.num = 1;
    avCodecCtx->time_base.den = 25;

    avcodec_parameters_from_context(avStream->codecpar, avCodecCtx);

    // 查找编码器
    avCodec = avcodec_find_encoder(avCodecCtx->codec_id);
    if (!avCodec)
    {
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        avio_close(avFormatCtx->pb);
        avformat_free_context(avFormatCtx);
        return -1;
    }

    // 设置avCodecCtx的编码器为avCodec
    if (avcodec_open2(avCodecCtx, avCodec, NULL) < 0)
    {
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        avio_close(avFormatCtx->pb);
        avformat_free_context(avFormatCtx);
        return -1;
    }

    //Write Header
    int ret = avformat_write_header(avFormatCtx, NULL);
    if (ret < 0)
    {
        printf("Error: %d\n", ret);
        avcodec_free_context(&avCodecCtx);
        avio_close(avFormatCtx->pb);
        avformat_free_context(avFormatCtx);
        return -1;
    }

    int size = avCodecCtx->width * avCodecCtx->height;

    // 给AVPacket分配足够大的空间
    AVPacket *pkt = av_packet_alloc();
    av_new_packet(pkt, size * 3);

    //Encode
    if (avcodec_send_frame(avCodecCtx, avFrame) == 0)
    {
        if (avcodec_receive_packet(avCodecCtx, pkt) == 0)
        {
            av_write_frame(avFormatCtx, pkt);

            //Write Trailer
            av_write_trailer(avFormatCtx);

            av_packet_free(&pkt);
            avcodec_free_context(&avCodecCtx);
            avio_close(avFormatCtx->pb);
            avformat_free_context(avFormatCtx);

            return 0;
        }
    }

    av_packet_free(&pkt);
    avcodec_free_context(&avCodecCtx);
    avio_close(avFormatCtx->pb);
    avformat_free_context(avFormatCtx);

    return -1;
}

//获取指定帧的画面保存为jpg文件
int getFrameJpg(AVInfo *av)
{
    //没有视频流，无法得到帧画面
    if (av->video_idx < 0)
    {
        printf("could not find video stream!\n");
        return -1;
    }

    //将目标时间戳转换到对应码流时基单位的时间戳
    AVRational tb;
    tb.num = 1;
    tb.den = AV_TIME_BASE;
    int64_t tar_pts = av->getFramePts * AV_TIME_BASE;
    tar_pts = av_rescale_q(tar_pts, tb, av->avFormatCtx->streams[av->video_idx]->time_base);

    //跳转到目标时间最近的关键帧
    av_seek_frame(av->avFormatCtx, av->video_idx, tar_pts, AVSEEK_FLAG_BACKWARD);

    //声明相关变量
    char url[] = "getFrame.jpg";     //暂定输出到这里
    AVCodecContext *avCodecCtx;
    AVCodec *avCodec = NULL;
    AVFrame *frame;
    AVPacket *pkt;

    //设置对应的解码器
    avCodecCtx = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecCtx, av->avFormatCtx->streams[av->video_idx]->codecpar);
    avCodec = avcodec_find_decoder(avCodecCtx->codec_id);//根据codec_id找到解码器
    if (avCodec == NULL)
    {
        printf("Could not find decoder!\n");
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        return -1;
    }
    if (avcodec_open2(avCodecCtx, avCodec, NULL) < 0)//打开解码器
    {
        printf("Could not open decoder!\n");
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        return -1;
    }

    //分配空间
    pkt = av_packet_alloc();
    frame = av_frame_alloc();

    //开始读取帧，直到找到第一帧视频帧
    while (av_read_frame(av->avFormatCtx, pkt) == 0)
    {
        if (pkt->stream_index == av->video_idx)
        {
            if (avcodec_send_packet(avCodecCtx, pkt) == 0)
            {
                while (avcodec_receive_frame(avCodecCtx, frame) == 0)
                {
                    //压缩成jpg格式输出
                    if (YUV2JPG(frame, url) < 0)
                    {
                        continue;
                    }

                    //释放空间
                    avcodec_close(avCodecCtx);
                    avcodec_free_context(&avCodecCtx);
                    av_packet_free(&pkt);
                    av_frame_free(&frame);
                    return 0;
                }
            }
        }
        else
        {
            av_packet_unref(pkt);//不是视频帧，释放空间
        }
    }

    avcodec_close(avCodecCtx);
    avcodec_free_context(&avCodecCtx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    return -1;
}

//获取波形图音频数据的后台处理线程
int LoadDataThread(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;

    for(;;)
    {
        SDL_LockMutex(av->abuf_mutex_waveform);
        SDL_CondWait(av->abuf_cond_write_waveform, av->abuf_mutex_waveform); //等待前端调用获取当前音频帧的函数
        if (av->quit)
        {
            av->audio_buf_size_waveform = -1;
            SDL_UnlockMutex(av->abuf_mutex_waveform);
            return 0;
        }


        //获取音频帧数据
        SDL_LockMutex(av->abuf_mutex);
        //SDL_CondWait(av->cond_waveform, av->abuf_mutex); //等待新的音频帧解码出来
        if (av->quit)
        {
            av->audio_buf_size_waveform = -1;
            SDL_UnlockMutex(av->abuf_mutex);
            SDL_UnlockMutex(av->abuf_mutex_waveform);
            return 0;
        }

        //访问共享区，进行处理
        av->audio_buf_size_waveform = av->audio_buf_size;
        memcpy_s(av->audio_buf_waveform, av->audio_buf_size_waveform, av->audio_buf, av->audio_buf_size);
        //处理结束，离开共享区
        SDL_UnlockMutex(av->abuf_mutex);

        SDL_CondSignal(av->abuf_cond_read_waveform);
        SDL_UnlockMutex(av->abuf_mutex_waveform);
    }

}

//视频解析线程函数，打开视频文件找到码流并打开音频播放和视频解码线程
int ParseThread(void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;
    AVPacket *avPacket;

    //初始化变量
    av->avFormatCtx = avformat_alloc_context();
    avPacket = av_packet_alloc();

    //打开视频文件
    if (avformat_open_input(&av->avFormatCtx, av->file_name, NULL, NULL) != 0)
    {
        printf("Could not open file!\n");
        avformat_close_input(&av->avFormatCtx);
        av_packet_free(&avPacket);
        return -1;
    }

    //获取视频文件信息
    if (avformat_find_stream_info(av->avFormatCtx, NULL) < 0)
    {
        printf("Could not find stream info!\n");
        av_packet_free(&avPacket);
        return -1;
    }

    //打印视频信息
    //av_dump_format(av->avFormatCtx, 0, NULL, 0);

    //找到视音频流下标
    av->video_idx = av->audio_idx = -1;
    for (unsigned int i = 0; i < av->avFormatCtx->nb_streams; ++i)
    {
        if (!av->type && av->avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && av->video_idx < 0)
        {
            av->video_idx = i;
            continue;
        }
        if (av->avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && av->audio_idx < 0)
        {
            av->audio_idx = i;
            continue;
        }
    }

    //没有视频和音频，退出
    if (av->video_idx == -1 && av->audio_idx == -1)
    {
        printf("Could not find video or audio streams!\n");
        av_packet_free(&avPacket);
        return -1;
    }

    //设置默认配置，其他参数默认值为0
    av->quit = 0;
    av->speed = 1;//默认原速播放
    av->volume = 128;//默认最大音量播放

    //打开解码视频流的相关组件，初始化av结构体中的相关视频信息
    if (av->video_idx >= 0 && av->video_idx < av->avFormatCtx->nb_streams)
    {
        av->vStream = av->avFormatCtx->streams[av->video_idx];
        av->vCodecCtx = avcodec_alloc_context3(av->vCodec);
        avcodec_parameters_to_context(av->vCodecCtx, av->vStream->codecpar);
        av->vCodec = avcodec_find_decoder(av->vCodecCtx->codec_id);//根据codec_id找到解码器
        if (av->vCodec == NULL)
        {
            printf("Could not find decoder!\n");
            av_packet_free(&avPacket);
            return -1;
        }
        if (avcodec_open2(av->vCodecCtx, av->vCodec, NULL) < 0)//打开解码器
        {
            printf("Could not open decoder!\n");
            av_packet_free(&avPacket);
            return -1;
        }
        //初始化数据
        av->last_vdts = INIT_DTS_VALUE;
        av->width = av->vCodecCtx->width;
        av->height = av->vCodecCtx->height;
        av->picq_mutex = SDL_CreateMutex();
        av->picq_cond_write = SDL_CreateCond();
        av->picq_cond_read = SDL_CreateCond();
        av->refresh_mutex = SDL_CreateMutex();
        av->refresh_cond = SDL_CreateCond();
        CreatePacketQueue(&av->video_queue);
        while (!av->screen)
        {
            SDL_Delay(1);
        }
        av->renderer = SDL_CreateRenderer(av->screen, -1, 0);
        av->texture = SDL_CreateTexture(av->renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, av->width, av->height);
        //设置视频图像格式转换规则
        av->swsCtx = sws_getContext(av->width, av->height, av->vCodecCtx->pix_fmt,
                                    av->width, av->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
        av->decode_video_tid = SDL_CreateThread(DecodeVideoThread, "DecodeVideoThread", av);//创建视频解码线程
        av->refresh_tid = SDL_CreateThread(VideoRefreshThread, "VideoRefreshThread", av);//创建视频刷新线程
        VideoRefreshTimer(av, 20);
        av->video_pts = 0;
        av->pre_vpts = 0;
        av->pre_vdelay = 40e-3;
        av->video_time = av_gettime() / 1000000.0 + 20;
    }

    //打开解码音频流的相关组件，初始化av结构体中的相关音频信息
    if (av->audio_idx >= 0 && av->audio_idx < av->avFormatCtx->nb_streams)
    {
        av->aStream = av->avFormatCtx->streams[av->audio_idx];
        av->aCodecCtx = avcodec_alloc_context3(av->aCodec);
        avcodec_parameters_to_context(av->aCodecCtx, av->aStream->codecpar);
        av->aCodec = avcodec_find_decoder(av->aCodecCtx->codec_id);//根据codec_id找到解码器
        if (av->aCodec == NULL)
        {
            printf("Could not find decoder!\n");
            av_packet_free(&avPacket);
            return -1;
        }
        if (avcodec_open2(av->aCodecCtx, av->aCodec, NULL) < 0)//打开解码器
        {
            printf("Could not open decoder!\n");
            av_packet_free(&avPacket);
            return -1;
        }
        av->aCodecCtx->pkt_timebase = av->aStream->time_base;
        //初始化数据
        av->last_adts = INIT_DTS_VALUE;
        av->audio_buf = (Uint8 *)av_mallocz(MAX_AUDIO_BUF_SIZE * 2);
        av->audio_buf_idx = 0;
        av->audio_buf_size = 0;
        av->abuf_mutex = SDL_CreateMutex();
        av->abuf_cond_read = SDL_CreateCond();
        av->abuf_cond_write = SDL_CreateCond();
        CreatePacketQueue(&av->audio_queue);
        //设置重采样规则
        int64_t in_channel_layout = av_get_default_channel_layout(av->aCodecCtx->channels);
        av->swrCtx = swr_alloc();
        swr_alloc_set_opts(av->swrCtx, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, av->aCodecCtx->sample_rate,
                           in_channel_layout, av->aCodecCtx->sample_fmt, av->aCodecCtx->sample_rate, 0, NULL);
        swr_init(av->swrCtx);
        //打开播放器
        SDL_AudioSpec wanted_spec;//播放参数
        wanted_spec.freq = av->aCodecCtx->sample_rate;//采样率
        wanted_spec.format = AUDIO_S16;//2bytes
        wanted_spec.channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);//立体声
        wanted_spec.samples = 1024;
        wanted_spec.callback = AudioCallback;
        wanted_spec.userdata = av;
        wanted_spec.silence = 0;
        if (SDL_OpenAudio(&wanted_spec, NULL))
        {
            printf("Could not open audio player!\n");
            av_packet_free(&avPacket);
            return -1;
        }
        av->decode_audio_tid = SDL_CreateThread(DecodeAudioThread, "DecodeAudioThread", av);//创建视频解码线程
        if (av->video_idx < 0 || av->video_idx >= av->avFormatCtx->nb_streams) //判断是否是纯音频文件，是的话创建波形图相关处理线程和变量
        {
            av->abuf_mutex_waveform = SDL_CreateMutex();
            av->abuf_cond_read_waveform = SDL_CreateCond();
            av->abuf_cond_write_waveform = SDL_CreateCond();
            av->cond_waveform = SDL_CreateCond();
            av->audio_buf_waveform = (Uint8 *)av_mallocz(MAX_AUDIO_BUF_SIZE * 2);
            av->waveform_tid = SDL_CreateThread(LoadDataThread, "LoadDataThread", av);
        }
        SDL_PauseAudio(0);
    }

    //获取每一帧数据包
    for (;;)
    {
        //判断是否退出播放
        if (av->quit)
        {
            av_packet_free(&avPacket);
            printf("user shuts the video!\n");
            return 0;
        }

        //判断是否需要获取某帧画面
        if (av->getFrameFlag == 1 && av->last_vdts > 0)
        {
            //暂时先去处理获取帧画面的请求
            av->getFrameFlag = getFrameJpg(av);

            //将av->avFormatCtx重新定位到之前读包位置附近继续读取压缩数据包
            av_seek_frame(av->avFormatCtx, av->video_idx, av->last_vdts, AVSEEK_FLAG_BACKWARD);
        }

        //判断是否需要跳转播放位置
        if (av->seeking == 1)
        {
            //选择有码流信息的码流作为跳转基准
            int stream_idx = av->video_idx >= 0 ? av->video_idx : av->audio_idx;

            //将目标时间戳转换到对应码流时基单位的时间戳
            AVRational tb;
            tb.num = 1;
            tb.den = AV_TIME_BASE;
            int64_t tar_pts = av->tar_pts * AV_TIME_BASE;
            tar_pts = av_rescale_q(tar_pts, tb, av->avFormatCtx->streams[stream_idx]->time_base);

            //跳转到目标时间最近的关键帧
            av_seek_frame(av->avFormatCtx, stream_idx, tar_pts, AVSEEK_FLAG_BACKWARD);

            //清空队列缓存并且加入flush_pkt包通知音视频解码线程有跳转操作
            if (av->video_idx >= 0)
            {
                ClearPacketQueue(&av->video_queue);
                PacketQueuePut(&av->video_queue, &flush_pkt);
                av->last_vdts = INIT_DTS_VALUE;
            }
            if (av->audio_idx >= 0)
            {
                ClearPacketQueue(&av->audio_queue);
                PacketQueuePut(&av->audio_queue, &flush_pkt);
                av->last_adts = INIT_DTS_VALUE;
            }

            av->seeking = 2;
        }

        //队列满就延迟添加
        if (av->video_queue.size >= MAX_VIDEO_PACKET_SIZE || av->audio_queue.size >= MAX_AUDIO_PACKET_SIZE)
        {
            SDL_Delay(100);
            continue;
        }

        //没有读取到帧，延迟40ms等待数据传入
        if (av_read_frame(av->avFormatCtx, avPacket) < 0)
        {
            SDL_Delay(40);
            continue;
        }

        //将视音频压缩数据包扔进对应的队列中等待播放
        if (avPacket->stream_index == av->video_idx && avPacket->dts > av->last_vdts)//视频数据包
        {
            av->last_vdts = avPacket->dts;
            PacketQueuePut(&av->video_queue, avPacket);
        }
        else if (avPacket->stream_index == av->audio_idx && avPacket->dts > av->last_adts)//音频数据包
        {
            av->last_adts = avPacket->dts;
            PacketQueuePut(&av->audio_queue, avPacket);
        }
        else//其他数据不做处理，清空包中的缓存
        {
            av_packet_unref(avPacket);
        }
    }

    return 0;
}

//释放图像缓存队列空间
void Free_Picture_Queue(AVInfo *av)
{
    for (int i = 0; i < PICTURE_QUEUE_SIZE; ++i)
    {
        VideoPicture *vp = &av->picture_queue[i];
        if (vp->buffer)
        {
            av_free(vp->buffer);
            vp->buffer = NULL;
        }
        if (vp->frameYUV)
        {
            av_frame_free(&vp->frameYUV);
        }
    }
}

//允许跳转操作定时器回调函数，防止用户频繁执行跳转操作
static Uint32 SeekEnableTimerCallBack(Uint32 interval, void *user_data)
{
    AVInfo *av = (AVInfo *)user_data;
    av->seeking = 0;
    return 0;
}

//创建视频
int CreateVideo(AVInfo *av)
{
    //创建视频显示窗口
    if (av->wid)
    {
        av->screen = SDL_CreateWindowFrom(av->wid);
    }
    else
    {
        av->screen = SDL_CreateWindow("MyPlayer", 50, 50, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    }
    SDL_ShowWindow(av->screen);
    av->screen_mutex = SDL_CreateMutex();

    if(av->wid) return 0;

    //等待消息到来
    SDL_Event event;
    for (;;)
    {
        SDL_WaitEventTimeout(&event, 100);
        if(av->quit)
        {
            return 0;
        }
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHTBRACKET://播放速度增加
                av->speed += 0.5;
                if (av->speed > 8)
                {
                    av->speed = 8;
                }
                break;
            case SDLK_LEFTBRACKET://播放速度减少
                av->speed -= 0.5;
                if (av->speed < 0.5)
                {
                    av->speed = 0.5;
                }
                break;
            case SDLK_LEFT://快退8秒
                if (!av->seeking)
                {
                    double tar_pts = GetAudioTime(av) - 8;
                    JumpToPts(av, tar_pts);
                    av->seeking = 1;
                    while (av->seeking != 2)
                    {
                        SDL_Delay(1);
                    }
                    SDL_AddTimer(300, SeekEnableTimerCallBack, av);
                }
                break;
            case SDLK_RIGHT://快进8秒
                if (!av->seeking)
                {
                    double tar_pts = GetAudioTime(av) + 8;
                    JumpToPts(av, tar_pts);
                    av->seeking = 1;
                    while (av->seeking != 2)
                    {
                        SDL_Delay(1);
                    }
                    SDL_AddTimer(300, SeekEnableTimerCallBack, av);
                }
                break;
            case SDLK_SPACE://播放/暂停
                av->pause = !av->pause;
                break;
            case SDLK_UP://提高音量
                av->volume += 2;
                if (av->volume > SDL_MIX_MAXVOLUME)
                {
                    av->volume = SDL_MIX_MAXVOLUME;
                }
                break;
            case SDLK_DOWN://降低音量
                av->volume -= 2;
                if (av->volume < 0)
                {
                    av->volume = 0;
                }
                break;
            case SDLK_f://全屏
                MyFullScreen(av);
                break;
            case SDLK_ESCAPE://关闭播放器
                if (av->fullScreen)
                {
                    MyFullScreen(av);
                }
                else//关闭视频
                {
                    Video_Quit(av);
                    return 0;
                }
                break;
            default:
                break;
            }
            break;
        case SDL_QUIT://关闭视频
            Video_Quit(av);
            return 0;
            break;
        default:
            break;
        }
    }
    return -1;
}

//SDL事件滤波器
int MySdlEventFilter(void *user_data, SDL_Event * event)
{
    AVInfo *av = (AVInfo *)user_data;

    if (event->type == SDL_WINDOWEVENT)
    {
        if (event->window.event == SDL_WINDOWEVENT_RESIZED || event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            av->resized = 2;
            return 0;
        }
    }
    return 1;
}

//构造函数
Player::Player()
{
    av = (AVInfo *)av_mallocz(sizeof(AVInfo));
    memset(av, 0, sizeof(AVInfo));
    av->quit = 1;
}

//析构函数
Player::~Player()
{
    if (av)
    {
        Init();
        av_free(av);
    }
}

//初始化类，清空av记录的信息以及释放其中申请的空间，并将av填充为0
void Player::Init()
{
    //释放视频格式上下文空间
    if (av->avFormatCtx)
    {
        avformat_close_input(&av->avFormatCtx);
    }

    //释放视音频解码器上下文空间
    if (av->vCodecCtx)
    {
        avcodec_close(av->vCodecCtx);
        avcodec_free_context(&av->vCodecCtx);
    }
    if (av->aCodecCtx)
    {
        avcodec_close(av->aCodecCtx);
        avcodec_free_context(&av->aCodecCtx);
    }

    //释放互斥锁变量，信号量等
    if (av->screen_mutex)
    {
        SDL_DestroyMutex(av->screen_mutex);
        av->screen_mutex = NULL;
    }
    if (av->picq_mutex)
    {
        SDL_DestroyMutex(av->picq_mutex);
        av->picq_mutex = NULL;
    }
    if (av->picq_cond_write)
    {
        SDL_DestroyCond(av->picq_cond_write);
        av->picq_cond_write = NULL;
    }
    if (av->picq_cond_read)
    {
        SDL_DestroyCond(av->picq_cond_read);
        av->picq_cond_read = NULL;
    }
    if (av->abuf_mutex)
    {
        SDL_DestroyMutex(av->abuf_mutex);
        av->abuf_mutex = NULL;
    }
    if (av->abuf_cond_write)
    {
        SDL_DestroyCond(av->abuf_cond_write);
        av->abuf_cond_write = NULL;
    }
    if (av->abuf_cond_read)
    {
        SDL_DestroyCond(av->abuf_cond_read);
        av->abuf_cond_read = NULL;
    }
    if (av->refresh_mutex)
    {
        SDL_DestroyMutex(av->refresh_mutex);
        av->refresh_mutex = NULL;
    }
    if (av->refresh_cond)
    {
        SDL_DestroyCond(av->refresh_cond);
        av->refresh_cond = NULL;
    }
    if (av->abuf_mutex_waveform)
    {
        SDL_DestroyMutex(av->abuf_mutex_waveform);
        av->abuf_mutex_waveform = NULL;
    }
    if (av->abuf_cond_write_waveform)
    {
        SDL_DestroyCond(av->abuf_cond_write_waveform);
        av->abuf_cond_write_waveform = NULL;
    }
    if (av->abuf_cond_read_waveform)
    {
        SDL_DestroyCond(av->abuf_cond_read_waveform);
        av->abuf_cond_read_waveform = NULL;
    }
    if (av->cond_waveform)
    {
        SDL_DestroyCond(av->cond_waveform);
        av->cond_waveform = NULL;
    }

    //销毁SDL相关组件
    if (av->screen)
    {
        SDL_DestroyWindow(av->screen);
        av->screen = NULL;
    }
    if (av->renderer)
    {
        SDL_DestroyRenderer(av->renderer);
        av->renderer = NULL;
    }
    if (av->texture)
    {
        SDL_DestroyTexture(av->texture);
        av->texture = NULL;
    }

    //销毁压缩数据包队列
    if (av->video_queue.qMutex)
    {
        DestroyPacketQueue(&av->video_queue);
    }
    if (av->audio_queue.qMutex)
    {
        DestroyPacketQueue(&av->audio_queue);
    }

    //释放图片缓存队列空间
    Free_Picture_Queue(av);

    //释放重采样、图片格式转换上下文
    if (av->swsCtx)
    {
        sws_freeContext(av->swsCtx);
        av->swsCtx = NULL;
    }
    if (av->swrCtx)
    {
        swr_free(&av->swrCtx);
    }

    //释放视音频缓存空间
    if (av->audio_buf)
    {
        av_free(av->audio_buf);
        av->audio_buf = NULL;
    }
    if (av->audio_buf_waveform)
    {
        av_free(av->audio_buf_waveform);
        av->audio_buf_waveform = NULL;
    }

    //将av置0
    memset(av, 0, sizeof(AVInfo));
    av->quit = 1;
}

//是否打开了视频文件
bool Player::Playing()
{
    return av->quit == 0;
}


//播放视频，输入视频文件路径和窗口控件的winID
void Player::Play(const char input_file[], bool isMusic, void * wid)
{
    if(Playing())
    {
        //确保关闭上个文件的播放线程
        Quit();
        SDL_Delay(250);
    }

    Init();//初始化类中参数

    //保存传入的参数信息
    strcpy_s(av->file_name, input_file);
    av->wid = wid;
    if(isMusic)
    {
        av->type = 1;
    }
    else
    {
        av->type = 0;
    }

    //初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        printf("Could not init SDL!\n");
        return;
    }
    //过滤窗口缩放事件
    SDL_SetEventFilter(MySdlEventFilter, av);

    //创建视频解析线程
    av->parse_tid = SDL_CreateThread(ParseThread, "ParseThread", av);

    //创建视频
    CreateVideo(av);

    //等待视频正常播放
    SDL_Delay(250);
}

//判断是否处于暂停状态，返回1表示暂停中，返回0表示播放中，返回-1表示没有打开任何视频
int Player::Pausing()
{
    if(!Playing()) return -1;
    return av->pause;
}

//暂停-播放切换
void Player::Pause()
{
    if(!Playing()) return;
    av->pause = !av->pause;
}

//获取视频的总长度，没有打开任何视频就返回-1
double Player::GetTotalDuration()
{
    if(!Playing()) return -1;
    return av->avFormatCtx->duration / 10000.0;
}

//获取当前播放进度，没有打开任何视频就返回-1
double Player::MyGetCurrentTime()
{
    if(!Playing()) return -1;
    return GetAudioTime(av) * 100;
}

//获取指定时刻的帧画面，单位10ms
bool Player::GetFrameJpg(double pts)
{
    pts /= 100.0;//转换为秒的单位
    if (pts < 0 || pts > av->avFormatCtx->duration / 1000000.0) return false;
    if (!Playing()) return false;
    if(av->video_idx < 0)
    {
        return false;
    }
    if (!av->getFrameFlag)
    {
        av->getFramePts = pts;
        av->getFrameFlag = 1;
        while (av->getFrameFlag == 1)
        {
            SDL_Delay(5);
        }
        if (av->getFrameFlag == 0)
        {
            return true;
        }
        av->getFrameFlag = 0;
        return false;
    }
    return false;
}

//跳转播放，输入跳转到的时间点，单位10ms，返回是否跳转成功
bool Player::Jump(double play_time)
{
    play_time /= 100.0;//转换为秒的单位
    if(play_time < 0 || play_time > av->avFormatCtx->duration / 1000000.0) return false;
    if(!Playing()) return false;
    if (!av->seeking)
    {
        JumpToPts(av, play_time);
        av->seeking = 1;
        while(av->seeking != 2)
        {
            SDL_Delay(5);
        }
        SDL_AddTimer(300, SeekEnableTimerCallBack, av);
        SDL_Delay(300);
        return true;
    }
    return false;
}

//快退
void Player::Backward(double t)
{
    if(!Playing()) return;
    if (!av->seeking)
    {
        double tar_pts = GetAudioTime(av) - t;
        JumpToPts(av, tar_pts);
        av->seeking = 1;
        while(av->seeking != 2)
        {
            SDL_Delay(5);
        }
        SDL_AddTimer(300, SeekEnableTimerCallBack, av);
        SDL_Delay(300);
    }
}

//快进
void Player::Forward(double t)
{
    if(!Playing()) return;
    if (!av->seeking)
    {
        double tar_pts = GetAudioTime(av) + t;
        JumpToPts(av, tar_pts);
        av->seeking = 1;
        while(av->seeking != 2)
        {
            SDL_Delay(5);
        }
        SDL_AddTimer(300, SeekEnableTimerCallBack, av);
        SDL_Delay(300);
    }
}

//设置播放速度，暂时限制[0.5, 8]，返回是否设置成功
bool Player::SetSpeed(double speed)
{
    if(speed < 0.5 || speed > 8) return false;
    if(!Playing()) return false;
    av->speed = speed;

    return true;
}

//加快播放速度
void Player::SpeedUp()
{
    if(!Playing()) return;
    av->speed += 0.5;
    if (av->speed > 8)
    {
        av->speed = 8;
    }
}

//降低播放速度
void Player::SpeedDown()
{
    if(!Playing()) return;
    av->speed -= 0.5;
    if (av->speed < 0.5)
    {
        av->speed = 0.5;
    }
}

//设置音量，输入值范围[0, 100]
bool Player::SetVolume(int volume)
{
    if(volume < 0 || volume > 100) return false;
    if(!Playing()) return false;
    av->volume = volume * SDL_MIX_MAXVOLUME / 100.0;

    return true;
}

//提高音量
void Player::VolumeUp()
{
    if(!Playing()) return;
    av->volume += 2;
    if (av->volume > SDL_MIX_MAXVOLUME)
    {
        av->volume = SDL_MIX_MAXVOLUME;
    }
}

//降低音量
void Player::VolumeDown()
{
    if(!Playing()) return;
    av->volume -= 2;
    if (av->volume < 0)
    {
        av->volume = 0;
    }
}

//全屏
void Player::FullScreen()
{
    if(!Playing()) return;
    MyFullScreen(av);
}

//退出播放器，即关闭视频文件
void Player::Quit()
{
    if(!Playing()) return;
    Video_Quit(av);
}

//获取当前播放音频帧的数据，存入*audio_buf中，并返回buf的size
int Player::GetAudioBuf(Uint8 **audio_buf)
{
    if (!Playing())
    {
        printf("There is not auido playing!\n");
        return -1;
    }
    if (av->video_idx >= 0)
    {
        return -1;
    }

    SDL_LockMutex(av->abuf_mutex_waveform);
    SDL_CondSignal(av->abuf_cond_write_waveform); //告诉后台线程可以去读取音频帧数据了
    SDL_UnlockMutex(av->abuf_mutex_waveform);

    SDL_LockMutex(av->abuf_mutex_waveform);

    SDL_CondWait(av->abuf_cond_read_waveform, av->abuf_mutex_waveform); //等待后台线程读取完音频帧数据再返回前端使用

    if (av->quit)
    {
        *audio_buf = NULL;
        SDL_UnlockMutex(av->abuf_mutex_waveform);
        return -1;
    }

    *audio_buf = av->audio_buf_waveform;

    SDL_UnlockMutex(av->abuf_mutex_waveform);
    return av->audio_buf_size;
}

//判断是不是视频
bool Player::isVideo()
{
    return av->video_idx != -1;
}

//提取视频文件的封面信息(输出视频的第一帧的JPG格式)
bool DrawJPG(const char file_path[], const char url[])
{
    //声明相关变量
    int video_idx = -1;
    AVFormatContext *avFormatCtx = avformat_alloc_context();
    AVCodecContext *avCodecCtx;
    AVCodec *avCodec = NULL;
    AVFrame *frame;
    AVPacket *pkt;

    //打开视频文件
    if (avformat_open_input(&avFormatCtx, file_path, NULL, NULL) != 0)
    {
        printf("Could not open file!\n");
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //找到文件流信息
    if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
    {
        printf("Could not find stream info!\n");
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //找到视频流下标
    for (int i = 0; i < int(avFormatCtx->nb_streams); ++i)
    {
        if (avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_idx = i;
            break;
        }
    }

    if(video_idx == -1)//找不到视频流，可能是纯音频文件
    {
        printf("Could not find video stream!\n");
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //设置对应的解码器
    avCodecCtx = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecCtx, avFormatCtx->streams[video_idx]->codecpar);
    avCodec = avcodec_find_decoder(avCodecCtx->codec_id);//根据codec_id找到解码器
    if (avCodec == NULL)
    {
        printf("Could not find decoder!\n");
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        avformat_close_input(&avFormatCtx);
        return false;
    }
    if (avcodec_open2(avCodecCtx, avCodec, NULL) < 0)//打开解码器
    {
        printf("Could not open decoder!\n");
        if (avCodecCtx)
        {
            avcodec_free_context(&avCodecCtx);
        }
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //分配空间
    pkt = av_packet_alloc();
    frame = av_frame_alloc();

    //开始读取帧，直到找到第一帧视频帧
    while (av_read_frame(avFormatCtx, pkt) == 0)
    {
        if (pkt->stream_index == video_idx)
        {
            if (avcodec_send_packet(avCodecCtx, pkt) == 0)
            {
                while(avcodec_receive_frame(avCodecCtx, frame) == 0)
                {
                    //压缩成jpg格式输出
                    if (YUV2JPG(frame, url) < 0)
                    {
                        continue;
                    }

                    //释放空间
                    avcodec_close(avCodecCtx);
                    avcodec_free_context(&avCodecCtx);
                    av_packet_free(&pkt);
                    av_frame_free(&frame);
                    avformat_close_input(&avFormatCtx);
                    return true;
                }
            }
        }
        else
        {
            av_packet_unref(pkt);//不是视频帧，释放空间
        }
    }

    avcodec_close(avCodecCtx);
    avcodec_free_context(&avCodecCtx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    avformat_close_input(&avFormatCtx);

    return false;
}

//给定文件路径，获取专辑封面数据，存入data中，并放回数据的大小，返回值为-1获取失败
int GetAlbumImg(const char *file_path, uint8_t *data)
{
    //声明相关变量
    AVFormatContext *avFormatCtx = avformat_alloc_context();

    //打开视频文件
    if (avformat_open_input(&avFormatCtx, file_path, NULL, NULL) != 0)
    {
        printf("Could not open file!\n");
        avformat_close_input(&avFormatCtx);
        return -1;
    }

    //找到文件流信息
    if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
    {
        printf("Could not find stream info!\n");
        avformat_close_input(&avFormatCtx);
        return -1;
    }

    //查找专辑图片
    for (int i = 0; i < int(avFormatCtx->nb_streams); ++i)
    {
        if (avFormatCtx->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
        {
            AVPacket pkt = avFormatCtx->streams[i]->attached_pic;
            memcpy_s(data, pkt.size, pkt.data, pkt.size);
            avformat_close_input(&avFormatCtx);
            return pkt.size;
        }
    }
    return -1;
}

//获取文件视音频流基本信息，返回获取成功与否
bool GetInfo(const char file_path[], VideoInfo *vi, AudioInfo *ai)
{
    //声明相关变量
    int video_idx = -1, audio_idx = -1;
    AVFormatContext *avFormatCtx = avformat_alloc_context();

    //打开视频文件
    if (avformat_open_input(&avFormatCtx, file_path, NULL, NULL) != 0)
    {
        printf("Could not open file!\n");
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //找到文件流信息
    if (avformat_find_stream_info(avFormatCtx, NULL) < 0)
    {
        printf("Could not find stream info!\n");
        avformat_close_input(&avFormatCtx);
        return false;
    }

    //找到视音频流下标
    for (int i = 0; i < int(avFormatCtx->nb_streams); ++i)
    {
        if (avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_idx == -1)
        {
            video_idx = i;
        }
        if (avFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_idx == -1)
        {
            audio_idx = i;
        }
    }

    //计算时长
    int duration = avFormatCtx->duration / 1000000;
    string dura;
    ostringstream os;
    int hours = duration / 3600;
    int mins = (duration / 60) % 60;
    int secs = duration % 60;
    if(hours < 10) os << '0';
    os << hours << ':';
    if(mins < 10) os << '0';
    os << mins << ':';
    if(secs < 10) os << '0';
    os << secs;
    dura = os.str();
    os.str("");

    //设置视频基本信息
    if (video_idx != -1)
    {
        AVCodecParameters *vCodecPar = avFormatCtx->streams[video_idx]->codecpar;
        vi->duration = dura.c_str();
        os << int(vCodecPar->bit_rate / 1000.0 + 0.5) << "kbps";
        vi->v_bit_rate = os.str().c_str(); os.str("");
        os << (int)av_q2d(avFormatCtx->streams[video_idx]->avg_frame_rate)<<"帧/秒";
        vi->frame_rate = os.str().c_str(); os.str("");
        os << vCodecPar->width << "x" << vCodecPar->height;
        vi->resolving = os.str().c_str(); os.str("");
    }

    //设置音频基本信息
    if (audio_idx != -1)
    {
        AVCodecParameters *aCodecPar = avFormatCtx->streams[audio_idx]->codecpar;
        ai->duration = dura.c_str();
        os << int(aCodecPar->bit_rate / 1000.0 + 0.5) << "kbps";
        ai->bit_rate = os.str().c_str(); os.str("");
        os << aCodecPar->channels;
        ai->channels = os.str().c_str(); os.str("");
        os << aCodecPar->sample_rate << "Hz";
        ai->sample_rate = os.str().c_str(); os.str("");

        //设置专辑信息
        AVDictionaryEntry *tag = nullptr;
        while ((tag = av_dict_get(avFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        {
            if(strcmp(tag->key, "album") == 0) {
                ai->album = tag->value;
            }
            else if(strcmp(tag->key, "artist") == 0) {
                ai->singer = tag->value;
            }
        }

    }

    avformat_close_input(&avFormatCtx);
    return true;
}
