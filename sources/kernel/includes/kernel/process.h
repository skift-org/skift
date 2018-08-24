#pragma once
#include "types.h"

#define PROC_STACKSZ 4096

typedef uint pid_t;

typedef struct
{
  pid_t pid;
  char name[128];
  int user;

  void * page_directorie;
  u32 esp;
  void * stack;
} process_t;

process_t * process_new(uint entry, page_directorie_t* page_directorie, int user);
pid_t process_exec(const char * path);
int process_kill(pid_t pid);