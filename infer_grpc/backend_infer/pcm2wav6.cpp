#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include "wav_utils.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << "Usage: ./pcm2wav <in_pcm_file> <out_sr> <out_wav_file>" << endl;
        return 1;
    }

    string pcm_file_name = argv[1];
    int out_sr = atoi(argv[2]);
    string wav_file_name = argv[3];

    cout << "pcm file: " << pcm_file_name << endl;
    cout << "out sr: " << out_sr << endl;
    cout << "wav file: " << wav_file_name << endl;

    int count_bytes = 0;
    char* pcm_raw_buffer = load_pcm_data(pcm_file_name.c_str(), count_bytes);
    if (pcm_raw_buffer == NULL)
    {
        cout << "failed to load pcm file: " << pcm_file_name << endl;
        return -1;
    }
    else
    {
        cout << "successfully loaded pcm file: " << pcm_file_name << endl;
        cout << "the num of samples in 16k pcm data is : " << count_bytes / 2 << endl;
    }

    int in_frames = count_bytes / 2;
    short* in_pcm_s16_buffer = (short*)pcm_raw_buffer;

    int out_actual_frames = 0;
    short* out_pcm_s16_buffer = NULL;
    bool is_ok = down_sample(in_pcm_s16_buffer, in_frames,
            out_actual_frames, out_pcm_s16_buffer);
    if(!is_ok)
    {
        cout << "error happened in down_sample()." << endl;
    }
    else
    {
        cout << "OK when to call down_sample()." << endl;
    }

    is_ok = save_pcm_as_wav(wav_file_name.c_str(), out_sr,
            out_pcm_s16_buffer, out_actual_frames);

    if(!is_ok)
    {
        cout << "error happened when save pcm as wav file." << endl;
    }
    else
    {
        cout << "OK when to save pcm as wav file." << endl;
    }

    if (out_pcm_s16_buffer != NULL)
        free(out_pcm_s16_buffer);

    return 0;
}
