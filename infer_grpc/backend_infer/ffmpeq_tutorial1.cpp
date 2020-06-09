//
// Created by Bill Fu on 2020/6/9.
//

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include <iostream>

using namespace std;

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}

int main(int argc, char* argv[])
{
    //av_register_all();

    AVFormatContext *pFormatCtx = NULL;

    // Open video file
    if(avformat_open_input(&pFormatCtx, argv[1], NULL,  NULL)!=0)
    {
        cout << "cannot open input file: " << argv[1] << endl;
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    // This function populates pFormatCtx->streams with the proper information.
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    {
        cout << "cannot find stream information." << endl;
        return -1; // Couldn't open file
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, argv[1], 0);

    int i;
    AVCodecContext *pCodecCtxOrig = NULL;
    AVCodecContext *pCodecCtx = NULL;

    // Find the first video stream
    int videoStream = -1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
        // AVCodecParameters* avCodecParameters = pFormatCtx->streams[i]->codecpar;
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }

    if(videoStream == -1)
    {
        cout << "cannot find a video stream." << endl;
        return -1; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    // pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    //...some code
    AVCodecParameters* avCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    pCodecCtx = avcodec_alloc_context3(NULL);
    if (!pCodecCtx)
    {
        cout << "alloc avcodec fail." << endl;
        return -1;
    }

    int ret = avcodec_parameters_to_context(pCodecCtx, avCodecParameters);
    if (ret < 0) {
        cout << "avcodec_parameters_to_context fail." << endl;
        return -1;
    }

    //The stream's information about the codec is in what we call
    // the "codec context." This contains all the information about
    // the codec that the stream is using, and now we have a pointer to it.
    // But we still have to find the actual codec and open it:
    AVCodec *pCodec = NULL;
    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        cout << "Unsupported codec!." << endl;
        return -1; // Codec not found
    }

    /*
    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0)
    {
        cout << "Couldn't copy codec context." << endl;
        return -1; // Error copying codec context
    }
    */

    // Open codec
    AVDictionary *options;
    if(avcodec_open2(pCodecCtx, pCodec, &options)<0)
    {
        cout << "Couldn't open codec." << endl;
        return -1; // Could not open codec
    }

    //Note that we must not use the AVCodecContext from
    // the video stream directly! So we have to use
    // avcodec_copy_context() to copy the context to a new
    // location (after allocating memory for it, of course).

    AVFrame *pFrame = NULL;

    // Allocate video frame
    pFrame=av_frame_alloc();

    // we're going to want to convert our initial frame to
    // a specific format. Let's allocate a frame for
    // the converted frame now.
    // Allocate an AVFrame structure
    AVFrame* pFrameRGB = av_frame_alloc();
    if(pFrameRGB == NULL)
        return -1;

    //Even though we've allocated the frame, we still need
    // a place to put the raw data when we convert it.
    // We use avpicture_get_size to get the size we need,
    // and allocate the space manually:
    uint8_t *buffer = NULL;

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
            pCodecCtx->width, pCodecCtx->height, 1);
    buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));

    //Now we use avpicture_fill to associate the frame with our newly
    // allocated buffer. About the AVPicture cast: the AVPicture struct
    // is a subset of the AVFrame struct - the beginning of the AVFrame
    // struct is identical to the AVPicture struct.

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
                         AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    //Finally! Now we're ready to read from the stream!

    struct SwsContext *sws_ctx = NULL;
    int frameFinished;
    AVPacket packet;
    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_RGB24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL
    );

    i=0;
    while(av_read_frame(pFormatCtx, &packet)>=0)
    {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream)
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if(frameFinished)
            {
                // Convert the image from its native format to RGB
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGB->data, pFrameRGB->linesize);

                // Save the frame to disk
                if(++i<=5)
                    SaveFrame(pFrameRGB, pCodecCtx->width,
                              pCodecCtx->height, i);
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    // Free the RGB image
    av_free(buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return 0;
}
