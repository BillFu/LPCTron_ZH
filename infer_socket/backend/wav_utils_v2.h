//
// Created by Bill Fu on 2020/6/10.
//

#ifndef BACKEND_INFER_WAV_UTILS_H
#define BACKEND_INFER_WAV_UTILS_H

typedef struct _riff_t
{
    char        riff[4];	/* "RIFF" (ASCII characters) */
    uint32_t    len;	/* Length of package (binary, little endian) */
    char        wave[4];	/* "WAVE" (ASCII characters) */
} riff_t;

/* The FORMAT chunk */
typedef struct _format_t
{
    char        fmt[4];		/* "fmt_" (ASCII characters) */
    uint32_t    len;	/* length of FORMAT chunk (always 0x10) */
    uint16_t    type;		/* codec type*/
    uint16_t    channel;	/* Channel numbers (0x01 = mono, 0x02 = stereo) */
    uint32_t    rate;	/* Sample rate (binary, in Hz) */
    uint32_t    bps;	/* Average Bytes Per Second */
    uint16_t    blockalign;	/*number of bytes per sample */
    uint16_t    bitpspl;	/* bits per sample */
} format_t;

typedef struct _data_t {
    char        data[4];   /* "data" (ASCII characters) */
    uint32_t    len;  /* length of data */
}data_t;

typedef struct _wav_head
{
    riff_t      riff;
    format_t    format;
    data_t      data;
}wav_head;

char* load_pcm_data(const char* pcm_file_name, int& count_bytes);

bool down_sample(const short* in_pcm_s16_buffer, int in_frames,
                 int& out_actual_frames, short*& out_pcm_s16_buffer);

bool save_pcm_as_wav(const char* wav_file_name, int sample_rate,
        short* pcm_s16_buffer, int num_samples);

#endif //BACKEND_INFER_WAV_UTILS_H
