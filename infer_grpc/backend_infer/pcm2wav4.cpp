
#include <cstdint>
#include <iostream>

using namespace std;


typedef struct _riff_t 
{
	char riff[4];	/* "RIFF" (ASCII characters) */
	uint32_t len;	/* Length of package (binary, little endian) */
	char wave[4];	/* "WAVE" (ASCII characters) */
} riff_t;

/* The FORMAT chunk */
typedef struct _format_t {
    char  fmt[4];		/* "fmt_" (ASCII characters) */
    uint32_t   len;	/* length of FORMAT chunk (always 0x10) */
    uint16_t  type;		/* codec type*/
    uint16_t channel;	/* Channel numbers (0x01 = mono, 0x02 = stereo) */
    uint32_t   rate;	/* Sample rate (binary, in Hz) */
    uint32_t   bps;	/* Average Bytes Per Second */
    uint16_t blockalign;	/*number of bytes per sample */
    uint16_t bitpspl;	/* bits per sample */
} format_t;

typedef struct _data_t {
    char data[4];   /* "data" (ASCII characters) */
    uint32_t len;  /* length of data */
}data_t;

typedef struct _wav_head
{
    riff_t      riff;
    format_t    format;
    data_t      data;
}wav_head;


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

    int channels = 1;
    int bits = 16;
    int sample_rate = 16000;

    //以下是为了建立.wav头而准备的变量
    wav_head header;
    FILE   *fp, *fpCpy;

    if ((fp = fopen(pcm_file_name.c_str(), "rb")) == NULL) //读取文件
    {
        printf("open pcm file %s error\n", argv[1]);
        return -1;
    }

    if ((fpCpy = fopen(wav_file_name.c_str(), "wb")) == NULL) //为转换建立一个新文件
    {
        printf("create wav file error\n");
        return -1;
    }

    //以下是创建wav头的HEADER;但len未定，因为不知道Data的长度。
    memcpy(header.riff.riff, "RIFF",4);
    memcpy(header.riff.wave, "WAVE",4);

    //以上是创建wav头的HEADER;
    if (ferror(fpCpy))
    {
        printf("error\n");
    }

    memcpy(&header.format.fmt, "fmt ", 4);
    header.format.len = 0x10;
    header.format.type = 0x1;
    header.format.channel = channels;
    header.format.rate = sample_rate;
    header.format.bps = sample_rate * bits/8 * channels;
    header.format.blockalign = bits/8 * channels;
    header.format.bitpspl = bits;

    memcpy(&header.data.data, "data", 4);

    fseek(fp, 0, SEEK_END);
    long count = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    cout << "length: " << count << endl;

    header.data.len = (uint32_t)count;
    header.riff.len = header.data.len + 36;

    int size = sizeof(header);
    //fwrite(&header, 1, sizeof(header), fpCpy);
    fwrite(&header, sizeof(header),1, fpCpy);

    char *buffer = (char *)malloc((count) * sizeof(char));
    fread(buffer, 1, count, fp);

    //fwrite(buffer, 1, count, fpCpy);
    fwrite(buffer, 1, count, fpCpy);


    free(buffer);
    fclose(fp); //关闭文件
    fclose(fpCpy);   //关闭文件

    return 0;
}

