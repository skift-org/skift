#pragma once

#include <skift/generic.h>
#include <skift/list.h>

typedef enum
{
    CONFIG_INT,
    CONFIG_FLOAT,
    CONFIG_STRING,
    CONFIG_BOOL,
    CONFIG_ARRAY,
} config_value_type_t;


typedef struct
{
    config_value_type_t type;

    union
    {
       int vint; 
       float vfloat;
       char* vstring;
       bool vbool;
       list_t* varray;
    };
} config_value_t;


typedef struct
{
    
} config_section_t;


typedef struct
{

} config_t;