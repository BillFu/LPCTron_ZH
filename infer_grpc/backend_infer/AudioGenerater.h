//
// Created by Bill Fu on 2020/5/29.
//

#ifndef BACKEND_INFER_AUDIOGENERATER_H
#define BACKEND_INFER_AUDIOGENERATER_H

#include <string>

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}

using namespace std;

class AudioGenerater
{
public:
    AudioGenerater();
    ~AudioGenerater();

    void generateAudioFile(const string& fileName, void* pcmData);

private:
    AVCodec* m_AudioCodec = nullptr;
    AVCodecContext* m_AudioCodecContext = nullptr;
    AVFormatContext* m_FormatContext = nullptr;
    AVOutputFormat* m_OutputFormat = nullptr;

    // init Format
    bool initFormat(const string& audioFileName);
};


#endif //BACKEND_INFER_AUDIOGENERATER_H
