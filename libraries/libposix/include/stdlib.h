#pragma once

#define RAND_MAX 0

void srand(unsigned int seed);
int rand(void);

double strtod(const char *nptr, char **endptr); 
__attribute__((noreturn)) void abort(void);

int system(const char *command);