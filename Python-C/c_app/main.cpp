#include <iostream>
#include <sys/msg.h>
#include <assert.h>

#include <string.h>

#define MAXSIZE 1024

struct vmsg_buf {
    long length;
    char message[MAXSIZE];
};


int main()
{
    std::cout << "Inter-Process Communication between Python and C" << std::endl;
    std::cout << "This is C application." << std::endl;

    int QueueKey = 8500;

    int qid = msgget(QueueKey, IPC_CREAT);
    assert(qid != -1);

    // firstly, waiting for the incoming message from python app
    struct vmsg_buf vmbuf_rev;
    vmbuf_rev.length = 1;
    memset(vmbuf_rev.message, 0, MAXSIZE);
    msgrcv(qid, (struct msgbuf *)&vmbuf_rev, MAXSIZE, vmbuf_rev.length, 0);

    std::cout << "received message: " << vmbuf_rev.message << std::endl;

    struct vmsg_buf vmbuf_send;
    vmbuf_send.length = 1;
    // memset(vmbuf.message, 0, MAXSIZE);
    char msg_text[] = "I am a C App.";
    size_t msg_len = strlen(msg_text);
    strncpy(vmbuf_send.message, msg_text, msg_len);

    int ret = msgsnd(qid, (struct msgbuf *)&vmbuf_send, MAXSIZE, 0);
    assert(ret != -1);

    ret = msgctl(qid, IPC_RMID, NULL);
    assert(ret != -1);

    return 0;
}
