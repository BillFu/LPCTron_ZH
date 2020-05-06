#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MSG_TYPE_A	10

struct msgbuf
{
    long mtype;       /* message type, must be > 0 */
    char mtext[100];    /* message data */
    int pid;
};


int main(void)
{
    key_t key;

    key = ftok(".", 1);
    if(key == -1)
    {
        perror("get key failed\n");
        return -1;
    }

    int msg_id;
    //获取消息队列的 ID
    msg_id = msgget(key, IPC_CREAT|0666);
    if(msg_id == -1)
    {
        perror("get msg id error\n");
        return -1;
    }

    char buffer[80];
    struct msgbuf msg;
    msg.mtype = MSG_TYPE_A;
    msg.pid = getpid();
    while(1)
    {
        fgets(msg.mtext, sizeof(msg.mtext), stdin);//含回车符

        msgsnd(msg_id, &msg, sizeof(msg), 0);

        if(strncmp(msg.mtext, "exit", 4) == 0)
        {
            break;
        }
    }

    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}
