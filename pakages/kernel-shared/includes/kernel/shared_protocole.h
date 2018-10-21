#pragma once

#include <skift/types.h>

#define MSGPAYLOAD_SIZE 1024
#define MSGNAME_SIZE 128

typedef struct 
{
    uint id;
    uint reply;
    char name[MSGNAME_SIZE];

    uint flags;
    
    uint to;
    uint from;

    void * payload;
    uint size;
} message_t;