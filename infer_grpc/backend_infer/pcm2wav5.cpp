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
    /*
    char* pcm_raw_buffer = load_pcm_data(pcm_file_name.c_str(), count_bytes);
    if (pcm_raw_buffer == NULL)
    {
        cout << "failed to load pcm file: " << pcm_file_name << endl;
        return -1;
    }
    else
    {
        cout << "successfully loaded pcm file: " << pcm_file_name << endl;
    }
    */

    ifstream ifs_pcm(pcm_file_name, std::ios::in | std::ifstream::binary);

    if ( !ifs_pcm ) {
        cout << "failed to open input pcm file: " << pcm_file_name << endl;
    }

    std::vector<short> in_pcm_buffer{};
    istreambuf_iterator<char> iter(ifs_pcm);
    std::copy(iter, std::istreambuf_iterator<char>(),
            std::back_inserter(in_pcm_buffer)); // this leaves newVector empty
    ifs_pcm.close(); // close explicilty for consistency

    short* in_pcm_s16_buffer = in_pcm_buffer.data();

    for(int i = 0; i< 256; i++ )
        cout << in_pcm_s16_buffer[i] << endl;

    cout << "---------------------"  << endl;

    int in_frames = in_pcm_buffer.size();
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

    for(int i = 0; i< 256; i++ )
        cout << out_pcm_s16_buffer[i] << endl;

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
