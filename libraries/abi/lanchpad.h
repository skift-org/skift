#pragma once

typedef enum
{

} LaunchpadFlag;

typedef struct
{
    char name[];
    char executable[];
    char *arguments[];
    int handles[];

    LaunchpadFlag flags;
} LaunchPad;
