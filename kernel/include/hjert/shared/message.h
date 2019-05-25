#pragma once

#define MSGPAYLOAD_SIZE 1024
#define MSGLABEL_SIZE 128

typedef struct 
{
    uint id;
    uint reply;
    char label[MSGLABEL_SIZE];

    uint flags;
    
    uint to;
    uint from;

    void * payload;
    uint size;
} message_t;