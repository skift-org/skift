#pragma once
#include "defs/size_t.h"
#include "defs/NULL.h"

void exit(int status);

void * malloc (size_t size);
void * realloc(void * p, size_t size);
void * calloc (size_t , size_t);
void   free   (void *);

int abs(int value);

/* !!! NOT STANDART --------------------------------------------------------- */

// string to uint
unsigned int stoi(const char * str, unsigned char base);

// uint to string
void itos(unsigned int value, char *buffer, unsigned char base);