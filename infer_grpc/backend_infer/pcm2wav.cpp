//
// Created by Bill Fu on 2020/5/29.
//

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include "AudioGenerater.h"

using namespace std;

int main(int argc, char** argv)
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

    ifstream pcm_input( pcm_file_name, ios::in|ios::binary);
    int count_bytes = 101120;

    // copies all data into buffer
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(pcm_input), {});

    void* pcmData = reinterpret_cast<void*>(buffer.data());

    AudioGenerater generater;
    bool is_ok = generater.generateAudioFile(wav_file_name, pcmData, count_bytes);
    if(is_ok)
        cout << "wav file generated successfully!" << endl;
    else
        cout << "failed to generate wav file!" << endl;

    return 0;
}