#pragma once

typedef long time_t;
typedef unsigned int clock_t;

clock_t clock();

time_t time(time_t *tloc);