#include <math.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

#include "arch.h"
#include "lpcnet.h"
#include "freq.h"


int main(int argc, char **argv)
{
    FILE *fin, *fout;
    LPCNetState *net;
    char mutex_semaphore_name[] = "/Mutex_SEM1";
    char feature_shmem_name[] = "/Feature_SHM1";
    int fd_feature_shmem = 0;
    sem_t* mutex_semaphore = NULL;
    void *pFeatureMemory = NULL;
    size_t feature_shmem_max_size = 2000 * 20 * 4;  // measured in bytes

    if (argc != 2)
    {
        fprintf(stderr, "usage: lpcnet_infer2 <output.pcm>\n");
        return 0;
    }

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
    rc = acquire_semaphore(MY_NAME, the_semaphore, params.live_dangerously);
    rc = sem_wait(pSemaphore);
    if (rc) {
        sprintf(s, "Acquiring the semaphore failed; errno is %d\n", errno);
        say(pName, s);
    }

    while (1)
    {
        float features[NB_FEATURES];
        short pcm[FRAME_SIZE];

        float in_features[NB_BANDS+2];
        fread(in_features, sizeof(features[0]), NB_BANDS+2, fin);
        if (feof(fin)) break;
        RNN_COPY(features, in_features, NB_BANDS);
        RNN_CLEAR(&features[18], 18);
        RNN_COPY(features+36, in_features+NB_BANDS, 2);

        lpcnet_synthesize(net, pcm, features, FRAME_SIZE);
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
    return 0;
}
