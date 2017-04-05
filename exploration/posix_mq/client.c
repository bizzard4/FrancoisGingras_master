#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>

#include "common.h"

int main(int argc, char **argv)
{
    mqd_t mq;
    char buffer[MAX_SIZE];

    /* open the mail queue */
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    printf("Send to server (enter \"exit\" to stop it):\n");

    // Send the struct
    struct Data d1;
    d1.v = 10;
    d1.c = 'z';
    d1.f = (92.0f/10.0f);

    memset(buffer, 0, MAX_SIZE);
    memcpy(buffer, &d1, sizeof(struct Data));
    mq_send(mq, buffer, MAX_SIZE, 0);


    // Send exit
    memset(buffer, 0, MAX_SIZE);
    strcpy(buffer, MSG_STOP);
    mq_send(mq, buffer, MAX_SIZE, 0);

    /* cleanup */
    mq_close(mq);

    return 0;
}