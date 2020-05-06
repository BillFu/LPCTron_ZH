
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SHM_SIZE	100

int main(void)
{
    key_t key;

    key = ftok(".", 1);
    if(key == -1)
    {
        perror("get key failed\n");
        return -1;
    }

    int shm_id;

    shm_id = shmget(key, SHM_SIZE, IPC_CREAT|0666);
    if(shm_id == -1)
    {
        perror("get shm id error\n");
        return -1;
    }

    char *addr;

    addr = shmat(shm_id, NULL, 0);
    if(addr == (void *)-1)
    {
        perror("map address error\n");
        return -1;
    }

    while(1)
    {
        fgets(addr, SHM_SIZE, stdin);
        if(strncmp(addr, "exit", 4) == 0)
        {
            break;
        }
        while( strncmp(addr, "ok", 2) != 0);
    }
    shmdt(addr);


    shmctl(shm_id , IPC_RMID, 0);

    return 0;
}