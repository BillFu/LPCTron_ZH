
//#include <stdafx.h>

#ifdef  __cplusplus
extern "C"
{
#endif

	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libavdevice/avdevice.h"
	#include "libavfilter/avfilter.h"
	//#include "libavfilter/avfiltergraph.h"
	#include "libavfilter/buffersink.h"
	#include "libavfilter/buffersrc.h"
	#include "libavutil/audio_fifo.h"
	#include "libavutil/avutil.h"
	#include "libavutil/fifo.h"
	#include "libswresample/swresample.h"

#ifdef  __cplusplus
}
#endif

//#include <conio.h>
#include <time.h>
//#include <tchar.h>
#include <stdio.h>
#include <cstring>

void write_wav_header(AVCodecContext *pCodecCtx, AVFormatContext *pFormatCtx, FILE *audioFile);

int open_input_file(const char* filename) ;

#define MAX_AUDIO_FRAME_SIZE 192000

uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;//声道格式
AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;//采样格式
AVFormatContext* ifmt_ctx = NULL;

int AudioStreamIndex = -1;
int got_frame ,ret = -1;  


char filename[] ="sample1.m4a";

int main()  
{  
	AVCodecContext *pCodecCtx;

	int channels=0;
	int sample_rate=0;

	AVPacket pkt_in, pkt_out;  

	AVFrame *frame = NULL;  
	uint8_t	 *buffer = NULL;

	struct SwrContext* audio_convert_ctx = NULL;
	char* extention;

	//av_register_all(); 

	if (open_input_file(filename) < 0)  
	{  
		printf("failed to open input file");  
		return 1;
	}  

	extention = strrchr(filename , '.') + 1;
	int isSame= strcmp(extention,"wav");

	FILE *p = NULL;  
	char tmpName[100];
	sprintf(tmpName, "%s_%d_%dchannel.wav", filename,
	        ifmt_ctx->streams[AudioStreamIndex]->codecpar->sample_rate,
	        ifmt_ctx->streams[AudioStreamIndex]->codecpar->channels);

	p = fopen(tmpName, "w+b");
    AVSampleFormat sample_format = (AVSampleFormat)(
            ifmt_ctx->streams[AudioStreamIndex]->codecpar->format);
    int size = av_get_bytes_per_sample(sample_format);
	av_dump_format(ifmt_ctx, 0, filename, 0);

	if(AudioStreamIndex >= 0)
	{
		 pCodecCtx = ifmt_ctx->streams[AudioStreamIndex]->codec;
		 printf("声道数:%d\n",pCodecCtx->channels);
		 printf("时长:%ld\n", ifmt_ctx->duration / 1000);
		 channels=pCodecCtx->channels;
		 sample_rate=pCodecCtx->sample_rate;
		 write_wav_header(pCodecCtx, ifmt_ctx, p);
	}

	//音频输出参数
	int out_nb_samples = pCodecCtx->frame_size;//nb_samples: AAC-1024 MP3-1152 
	int out_sample_rate =pCodecCtx->sample_rate;// 44100;//采样率
	int out_nb_channels = av_get_channel_layout_nb_channels(out_channel_layout);//根据声道格式返回声道个数
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_nb_channels, out_nb_samples, out_sample_fmt, 1);
 
	buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
	audio_convert_ctx = swr_alloc();
	if (audio_convert_ctx == NULL)
	{
		printf("Could not allocate SwrContext\n");
		return -1;
	}
	swr_alloc_set_opts(audio_convert_ctx, out_channel_layout, out_sample_fmt,out_sample_rate,pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, NULL);
	swr_init(audio_convert_ctx);
	int frameindex=0;

	while(1)  
	{  
		if (av_read_frame(ifmt_ctx, &pkt_in) < 0)  
		{  
			break;  
		}  
		av_init_packet(&pkt_out);  
		if (AudioStreamIndex == pkt_in.stream_index)
		{
			frame = av_frame_alloc();  
			ret = avcodec_decode_audio4(ifmt_ctx->streams[AudioStreamIndex]->codec, frame, &got_frame, &pkt_in);  
			if (ret < 0)  
			{  
				av_frame_free(&frame);  
				printf("decoding audio stream failed\n");  
				break;  
			}  
			if (got_frame)  
			{
				frameindex++;

				if(isSame)
				{
				 
				  swr_convert(audio_convert_ctx, &buffer, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)frame->data, frame->nb_samples<1000?1152:frame->nb_samples);
				  printf("index:%5d\t pts:%lld\t packet size:%d\n", frameindex, pkt_in.pts, pkt_in.size);
				  fwrite(buffer, 1, out_buffer_size, p);
				}
				else
				{
					if(frame->data[0])
					{
						 fwrite(frame->data[0], 1, frame->linesize[0], p);
					}
				}
			}  
		}       
	}  
	printf("总帧数:%d\n",frameindex);
	fclose(p);  
	end:  
    swr_free(&audio_convert_ctx);
	av_free(buffer);
	avcodec_close(pCodecCtx);
	avformat_close_input(&ifmt_ctx);  
	printf("enter any key to stop\n");
	//getchar();  
	return 0;  
}  

int open_input_file(const char* filename)  
{  
	//open the input  
	if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0)  
	{  
	    printf("can not open input");  
	    return ret;  
	}  
	if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)))  
	{  
	    printf("can not find input stream info");  
	    return ret;  
	}  

	//open the decoder  
	for (int i = 0; i <(int)ifmt_ctx->nb_streams; i++)  
	{  
	    if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)  
	    {  
	        AudioStreamIndex = i;
	        ret = avcodec_open2(ifmt_ctx->streams[i]->codec,avcodec_find_decoder(ifmt_ctx->streams[i]->codec->codec_id), NULL);  
	        if (ret < 0)  
	        {  
	            printf("can not open decoder");  
	            return ret;  
	        } 
	    }  
	}

	return 0;  
}  

void write_wav_header(AVCodecContext *pCodecCtx, AVFormatContext *pFormatCtx, FILE *audioFile)
{
    //wav文件有44字节的wav头，所以要写44字节的wav头
    int8_t *data;
    uint32_t long_temp;
    uint16_t short_temp;
    uint16_t BlockAlign;
    int bits = 8;
    uint64_t fileSize;
    uint64_t audioDataSize;

	switch (pCodecCtx->sample_fmt) 
	{
    case AV_SAMPLE_FMT_S16:
	    bits = 16;
	    break;
   case AV_SAMPLE_FMT_S32:
		bits = 32;
		break;
   case AV_SAMPLE_FMT_U8:
	   bits = 8;
		break;
   default:
		bits = 16;
		break;
	}
	audioDataSize = (pFormatCtx->duration)*(bits / 8)*(pCodecCtx->sample_rate)*(pCodecCtx->channels);
	fileSize = audioDataSize + 36;
	data = (int8_t *)"RIFF";
	fwrite(data, sizeof(char), 4, audioFile);
	fwrite(&fileSize, sizeof(int32_t), 1, audioFile);
	data = (int8_t *)"WAVE";
	fwrite(data, sizeof(char), 4, audioFile);
	data = (int8_t *)"fmt ";
	fwrite(data, sizeof(char), 4, audioFile);
	long_temp = 16;
	fwrite(&long_temp, sizeof(int32_t), 1, audioFile);
	short_temp = 0x01;
	fwrite(&short_temp, sizeof(int16_t), 1, audioFile);
	short_temp = (pCodecCtx->channels);
	fwrite(&short_temp, sizeof(int16_t), 1, audioFile);
	long_temp = (pCodecCtx->sample_rate);
	fwrite(&long_temp, sizeof(int32_t), 1, audioFile);
	long_temp = (bits / 8)*(pCodecCtx->channels)*(pCodecCtx->sample_rate);
	fwrite(&long_temp, sizeof(int32_t), 1, audioFile);
	BlockAlign = (bits / 8)*(pCodecCtx->channels);
	fwrite(&BlockAlign, sizeof(int16_t), 1, audioFile);
	short_temp = (bits);
	fwrite(&short_temp, sizeof(int16_t), 1, audioFile);
	data = (int8_t *) "data";
	fwrite(data, sizeof(char), 4, audioFile);
	fwrite(&audioDataSize, sizeof(int32_t), 1, audioFile);
}