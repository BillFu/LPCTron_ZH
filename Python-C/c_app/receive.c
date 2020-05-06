
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#define MSG_TYPE	0
#define MSG_TYPE_A	10
#define MSG_TYPE_B	11

//自定义，方便按照消息类型进行处理
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
    msg_id = msgget(key, IPC_CREAT|0666);//不存在则创建，并设置权限为0666
    if(msg_id == -1)
    {
        perror("get msg id error\n");
        return -1;
    }

    struct msgbuf msg;
    int retval;

    while(1)
    {
        retval = msgrcv(msg_id,&msg, sizeof(msg), MSG_TYPE, IPC_NOWAIT);//IPC_NOWAIT，不等待，即不阻塞
        if((retval == -1)&&(errno != ENOMSG))//ENOMSG 还没有信息
        {
            perror("rcv error\n");
            return -1;
        }
        else if(retval != -1)
        {
            printf("[PID:%d], msg type:%ld, msg:%s\n", msg.pid, msg.mtype, msg.mtext);

            //可以指定长度为4时strlen(msg.mtext)才进行比较，仅仅对exit生效
            if(strncmp(msg.mtext, "exit", 4) == 0)
            {
                break;
            }
        }
    }

    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}
