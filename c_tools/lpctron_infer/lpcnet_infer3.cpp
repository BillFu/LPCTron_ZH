#include <math.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <iostream>
//#include <nlohmann/json.hpp>

using namespace std;
// using nlohmann::json

#include "arch.h"
#include "lpcnet.h"
#include "freq.h"

#define MAX_CMN_LEN 400
#define JOBDES_SECTION_SIZE 1024

int main(int argc, char **argv)
{
    FILE *fin, *fout;
    LPCNetState *net;
    char mutex_semaphore_name[] = "/Mutex_SEM1";
    char feature_shmem_name[] = "/Feature_SHM1";
    int fd_feature_shmem = 0;
    sem_t* mutex_semaphore = NULL;
    void *pFeatureMemory = NULL;
    size_t feature_shmem_max_size = JOBDES_SECTION_SIZE + 2000 * 20 * 4;  // measured in bytes
    int rc;
    char ffmpeg_cmd[MAX_CMN_LEN] = "";

    if (argc != 3)
    {
        fprintf(stderr, "usage: lpcnet_infer2 <output.pcm> <output.wav> \n");
        return 0;
    }

    /*
    if(!(strcmp(argv[2], "16k") == 0 ||
        (strcmp(argv[2], "8k") == 0)))
    {
        fprintf(stderr, "usage: lpcnet_infer2 <output.pcm> <16k or 8k> <output.wav> \n");
        return 0;
    }
    */

    net = lpcnet_create();

    // Mrs. Premise has already created the semaphore and shared memory.
    // I just need to get handles to them.
    mutex_semaphore = sem_open(mutex_semaphore_name, 0);
    if (mutex_semaphore == SEM_FAILED) {
        mutex_semaphore = NULL;
        printf("Failed to get a handle to the mutex semaphore; errno is %d", errno);
        exit(1);
    }

    // get a handle to the shared memory to pass features data
    fd_feature_shmem = shm_open(feature_shmem_name, O_RDONLY, 0666);
    if (fd_feature_shmem == -1) {
        printf("Failed to get the handle to the shared memory for features; errno is %d", errno);
        exit(1);
    }

    // mmap it.
    pFeatureMemory = mmap((void *)NULL, feature_shmem_max_size,
            PROT_READ, MAP_SHARED, fd_feature_shmem, 0);
    if (pFeatureMemory == MAP_FAILED)
    {
        printf("Failed to mmap the shared memory of features; errno is %d", errno);
        exit(1);
    }

    fout = fopen(argv[1], "wb");
    if (fout == NULL)
    {
	    fprintf(stderr, "Can't open %s\n", argv[2]);
	    exit(1);
    }

    // Wait for Mrs. Premise to free up the semaphore.
    rc = sem_wait(mutex_semaphore);
    if (rc) {
        fprintf(stderr, "Acquiring the semaphore failed; errno is %d\n", errno);
    }

    char* job_des_str = (char*)pFeatureMemory;
    cout << "job description section: " << endl << job_des_str << endl;

    float* pFeatureArray = (float*)((char*)pFeatureMemory + JOBDES_SECTION_SIZE);
    for(int i=0; i<500; i++)  // iterate over 500 frames
    {
        float in_features[NB_FEATURES];  // NB_FEATURES is 38, the data will enter lpcnet
        short pcm[FRAME_SIZE]; // output pcm data produced by lpcnet.

        float* raw_features = pFeatureArray + i*(NB_BANDS+2);  // NB_BANDS+2 == 20

        RNN_COPY(in_features, raw_features, NB_BANDS);
        RNN_CLEAR(&in_features[18], 18);
        RNN_COPY(in_features+36, raw_features+NB_BANDS, 2);

        lpcnet_synthesize(net, pcm, in_features, FRAME_SIZE);
        fwrite(pcm, sizeof(pcm[0]), FRAME_SIZE, fout);
    }

    rc = sem_post(mutex_semaphore);
    if (rc)
    {
        fprintf(stderr, "Failed to release the mutex semaphore; errno is %d\n", errno);
    }

    rc = sem_unlink(mutex_semaphore_name);
    if (rc) {
        fprintf(stderr, "failed to unlink the mutex semaphore; errno is %d", errno);
    }

    rc = shm_unlink(feature_shmem_name);
    if (rc) {
        fprintf(stderr, "Failed to unlink the shared memory; errno is %d", errno);
    }

    fclose(fout);
    lpcnet_destroy(net);

    // the final step is convert pcm data to wav file,
    // and re-sampling from 16k to 8K if needed by drive ffmpeg.
    // ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm1.f16 -ar 16k synth_16k_1.wav
    // ffmpeg -f s16le -ar 16k -ac 1 -i synth_pcm1.f16 -ar 8k  synth_8k_1.wav

    // argv[1] is the name of pcm file output.
    // argv[2] is "16k" or "8k"
    // argv[3] is the name of wav file output.
    //if(strcmp(argv[2], "16k") == 0)
    sprintf(ffmpeg_cmd, "ffmpeg -f s16le -ar 16k -ac 1 -i %s -ar 16k %s",
            argv[1],  argv[2]);
    /*
    else  // 8k for the output wav file
        sprintf(ffmpeg_cmd, "ffmpeg -f s16le -ar 16k -ac 1 -i %s -ar 8k %s",
                argv[1],  argv[3]);
    */
    system(ffmpeg_cmd);

    return 0;
}
