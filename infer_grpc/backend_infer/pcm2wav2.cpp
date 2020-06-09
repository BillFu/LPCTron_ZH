// WAVWriter.cpp: Defines the entry point for the console application.
//
 
//#include <stdafx.h>

#include <iostream>
 
using namespace std;
 
typedef struct WAVE_HEADER 
{
    //RIFF
    char chunkID[4];
         //long int 4 bytes a certain size
    unsigned long chunkSize;
    //WAVE
    char formate[4];
}WAVE_HEADER;

typedef struct WAVE_FMT 
{
         //fmt notice that there is a space at the end
    char subchunk1ID[4];
    unsigned long subchunk1Size;
    unsigned short audioFormatTag;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitPerSample;
}WAVE_FMT;

typedef struct WAVE_DATA {
    char subchunk2Id[4];
    unsigned long subchunk2Size;
}WAVE_DATA;
 
long getFileSize(char* filename) 
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}
 
int converToWAV(const char *pcmpath, int channels, 
    int sample_rate, const char *wavepath) 
{
    int bits = 16;
    WAVE_HEADER   pcmHEADER;
    WAVE_FMT   pcmFMT;
    WAVE_DATA   pcmDATA;
    FILE *fp, *fpout;
 
    fp = fopen(pcmpath, "rb");
    if (NULL == fp) 
    {
        std::cout << "file open failed" << endl;
        return -1;
    }
    long pcmSize = getFileSize((char*)pcmpath);
    //WAVE_HEADER
    memcpy(pcmHEADER.chunkID, "RIFF", strlen("RIFF"));
    pcmHEADER.chunkSize = 36 + pcmSize;
    memcpy(pcmHEADER.formate, "WAVE", strlen("WAVE"));
 
 
    fpout = fopen(wavepath, "wb");
    if (NULL == fpout) {
        cout << "file open failed" << endl;
        return -1;
    }
    fwrite(&pcmHEADER, sizeof(pcmHEADER), 1, fpout);

    //WAVE_FMT
    pcmFMT.numChannels = channels;
    pcmFMT.sampleRate = sample_rate;
    pcmFMT.bitPerSample = bits;
    pcmFMT.byteRate = sample_rate*channels *pcmFMT.bitPerSample / 8;
    memcpy(pcmFMT.subchunk1ID, "fmt ", strlen("fmt "));
    pcmFMT.subchunk1Size = 16;
    pcmFMT.audioFormatTag = 1;
    pcmFMT.blockAlign = channels*pcmFMT.bitPerSample / 8;
    fwrite(&pcmFMT, sizeof(pcmFMT), 1, fpout);
 
    //WAVE_DATA
    memcpy(pcmDATA.subchunk2Id, "data", strlen("data"));
    pcmDATA.subchunk2Size = pcmSize;
    fwrite(&pcmDATA, sizeof(pcmDATA), 1, fpout);
    char *buff=(char*)malloc(512);
    int len;
    while ((len = fread(buff, sizeof(char), 512, fp)) != 0)
    {
        fwrite(buff, sizeof(char), len, fpout);
    }
    free(buff);
    fclose(fp);
    fclose(fpout);

    return 1;
} 
 
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

    converToWAV(pcm_file_name.c_str(), 1, out_sr, wav_file_name.c_str());
    return 0;
}