import sysv_ipc

QueueKey = 8500
try:
    mq = sysv_ipc.MessageQueue(QueueKey, sysv_ipc.IPC_CREX)
except sysv_ipc.ExistentialError:
    print("ERROR: message queue creation failed")

mq.send("message to C program", True)

message = mq.receive()
print("message received from python:{}", message)

mq.remove()
