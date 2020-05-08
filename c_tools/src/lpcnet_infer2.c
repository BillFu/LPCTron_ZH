#include <math.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

#include "arch.h"
#include "lpcnet.h"
#include "freq.h"


int main(int argc, char **argv) {
    FILE *fin, *fout;
    LPCNetState *net;
    char semaphore_name[] = "/LPCTron_SEM1";
    char shmem_name[] = "/LPCTron_SHM1";
    int fd_shmem = 0;
    sem_t* ipc_semaphore = NULL;
    void *pSharedMemory = NULL;
    size_t shmem_max_size = 2000 * 20 * 4;  // measured in bytes

    if (argc != 2)
    {
        fprintf(stderr, "usage: test_lpcnet <output.pcm>\n");
        return 0;
    }

    net = lpcnet_create();

    // Mrs. Premise has already created the semaphore and shared memory.
    // I just need to get handles to them.
    ipc_semaphore = sem_open(semaphore_name, 0);

    if (ipc_semaphore == SEM_FAILED) {
        ipc_semaphore = NULL;
        printf("Getting a handle to the semaphore failed; errno is %d", errno);
        exit(1);
    }

    // get a handle to the shared memory
    fd_shmem = shm_open(shmem_name, O_RDONLY, 0666);
    if (fd_shmem == -1) {
        printf("Failed to get a handle to the shared memory; errno is %d", errno);
        exit(1);
    }

    // mmap it.
    pSharedMemory = mmap((void *)NULL, shmem_max_size,
            PROT_READ, MAP_SHARED, fd_shmem, 0);
    if (pSharedMemory == MAP_FAILED)
    {
        printf("Failed to mmap the shared memory; errno is %d", errno);
        exit(1);
    }


            i = 0;
            done = 0;
            last_message_i_wrote[0] = '\0';
            while (!done) {
                sprintf(s, "iteration %d", i);
                say(MY_NAME, s);

                // Wait for Mrs. Premise to free up the semaphore.
                rc = acquire_semaphore(MY_NAME, the_semaphore, params.live_dangerously);
                if (rc)
                    done = 1;
                else {
                    while ( (!rc) && \
                                (!strcmp((char *)pSharedMemory, last_message_i_wrote))
                            ) {
                        // Nothing new; give Mrs. Premise another change to respond.
                        sprintf(s, "Read %zu characters '%s'", strlen((char *)pSharedMemory), (char *)pSharedMemory);
                        say(MY_NAME, s);
                        say(MY_NAME, "Releasing the semaphore");
                        rc = release_semaphore(MY_NAME, the_semaphore, params.live_dangerously);
                        if (!rc) {


    rc = shm_unlink(params.shared_memory_name);
    if (rc) {
        sprintf(s, "Unlinking the memory failed; errno is %d", errno);
        say(MY_NAME, s);
    }

    fout = fopen(argv[1], "wb");
    if (fout == NULL)
    {
	    fprintf(stderr, "Can't open %s\n", argv[2]);
	    exit(1);
    }

    while (1)
    {

        float features[NB_FEATURES];
        short pcm[FRAME_SIZE];

#ifndef TACOTRON2
        float in_features[NB_TOTAL_FEATURES];
        fread(in_features, sizeof(features[0]), NB_TOTAL_FEATURES, fin);
        if (feof(fin)) break;
        RNN_COPY(features, in_features, NB_FEATURES);
        RNN_CLEAR(&features[18], 18);
#else
        float in_features[NB_BANDS+2];
        fread(in_features, sizeof(features[0]), NB_BANDS+2, fin);
        if (feof(fin)) break;
        RNN_COPY(features, in_features, NB_BANDS);
        RNN_CLEAR(&features[18], 18);
        RNN_COPY(features+36, in_features+NB_BANDS, 2);
#endif
        lpcnet_synthesize(net, pcm, features, FRAME_SIZE);
        fwrite(pcm, sizeof(pcm[0]), FRAME_SIZE, fout);
    }

    fclose(fout);
    lpcnet_destroy(net);
    return 0;
}
