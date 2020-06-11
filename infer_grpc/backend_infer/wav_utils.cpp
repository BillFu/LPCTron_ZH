
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include <samplerate.h>
#include "wav_utils.h"

using namespace std;


char* load_pcm_data(const char* pcm_file_name, int& count_bytes)
{
    FILE* file_pcm;
    file_pcm = fopen(pcm_file_name, "rb");
    if (file_pcm == NULL) //读取文件
    {
        // cout << "failed to open pcm file: " << pcm_file_name << endl;
        return NULL;
    }

    fseek(file_pcm, 0, SEEK_END);
    count_bytes = ftell(file_pcm);
    fseek(file_pcm, 0, SEEK_SET);

    cout << "the length of pcm file (in bytes): " << count_bytes << endl;

    char* pcm_raw_buffer = (char*)malloc(count_bytes * sizeof(char));
    fread(pcm_raw_buffer, 1, count_bytes, file_pcm);

    // free(buffer);
    fclose(file_pcm); //关闭文件

    return pcm_raw_buffer;
}


bool down_sample(const short* in_pcm_s16_buffer, int in_frames,
        int& out_actual_frames, short*& out_pcm_s16_buffer)
{
    SRC_DATA src_data;
    memset (&src_data, 0, sizeof (src_data));

    int out_frames = in_frames >> 1;

    float* in_pcm_float_buffer = (float*)malloc(in_frames * sizeof(float));
    float* out_pcm_float_buffer = (float*)malloc(out_frames * sizeof(float));

    src_short_to_float_array(in_pcm_s16_buffer, in_pcm_float_buffer, in_frames);

    src_data.end_of_input = 0;
    src_data.data_in = in_pcm_float_buffer;
    src_data.input_frames = in_frames;
    src_data.src_ratio = 0.5;
    src_data.data_out = out_pcm_float_buffer;
    src_data.output_frames = out_frames;
    /*
        SRC_SINC_BEST_QUALITY       = 0,
        SRC_SINC_MEDIUM_QUALITY     = 1,
        SRC_SINC_FASTEST            = 2,
        SRC_ZERO_ORDER_HOLD         = 3,
        SRC_LINEAR                  = 4
    */
    int error = src_simple (&src_data, SRC_SINC_BEST_QUALITY, 1);
    if( error != 0)
    {
        cout << "ERROR happened when to call src_simple(): " << src_strerror(error) << endl;
        return false;
    }

    out_actual_frames = src_data.output_frames_gen;
    out_pcm_s16_buffer = (short*)malloc(out_actual_frames * sizeof(short));
    src_float_to_short_array(out_pcm_float_buffer, out_pcm_s16_buffer, out_actual_frames);

    free(in_pcm_float_buffer);
    free(out_pcm_float_buffer);

    return true;
}

bool save_pcm_as_wav(const char* wav_file_name, int sample_rate,
                     short* pcm_s16_buffer, int num_samples)
{
    wav_head header;
    int bits = 16;

    FILE* file_wav;

    if ((file_wav = fopen(wav_file_name, "wb")) == NULL) //为转换建立一个新文件
    {
        cout << "cannot create output wav file: " << wav_file_name << endl;
        return false;
    }

    //以下是创建wav头的HEADER;但.len未定，因为不知道Data的长度。
    memcpy(header.riff.riff, "RIFF",4);
    memcpy(header.riff.wave, "WAVE",4);

    memcpy(&header.format.fmt, "fmt ", 4);
    header.format.len = 0x10;
    header.format.type = 0x1;
    header.format.channel = 1;
    header.format.rate = sample_rate;
    header.format.bps = sample_rate * bits/8 ;
    header.format.blockalign = bits/8;
    header.format.bitpspl = bits;

    memcpy(&header.data.data, "data", 4);

    int bytes_of_samples = num_samples * sizeof(short);
    header.data.len = (uint32_t)bytes_of_samples;
    header.riff.len = header.data.len + 36;

    int size = sizeof(header);
    fwrite(&header, sizeof(header), 1, file_wav);
    fwrite(pcm_s16_buffer, sizeof(short), num_samples, file_wav);

    fclose(file_wav);

    return true;
}
