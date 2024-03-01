#include <stdio.h>
#include <string.h>
/*
* Copyright (C) 2024 AnatoliyL.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Andesaurus Free Software License version 1.1-no-mintsuki-clause as published by AnatoliyL.
* See COPYING file for more details.
*/
int main(int argc, char **argv)
{
  for (int i = 0; i < strlen(argv[1]); i++)
    {
      if (argv[1][i] == argv[2][0])
        argv[1][i] = argv[3][0];
    }
  printf("%s\n", argv[1]);
  return (0);
}
