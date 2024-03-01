/*
* Copyright (C) 2024 AnatoliyL.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Andesaurus Free Software License version 1.1 as published by AnatoliyL.
* See COPYING file for more details.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void deleten(char *);
void deleten(char *bar)
{
  bar[strlen(bar) - 1] = '\0';
}
int main(void)
{
  puts("\t\tListis\tCopyright (C) 2024 AnatoliyL\n");
  puts("This program comes with ABSOLUTELY NO WARRANTY\nThis is free software, and you are welcome to redistribute it under GNU GPL license conditions\n");
  for (int i = 0; i < 5; i++)
    putchar('\n');
  puts("To watch existing list, press 'o', to create new, press 'n'\n");
  int v = getchar();
  if (v == 'o')
    {
      char name[512];
      fgets(name, 512, stdin);
      deleten(name);
      FILE *f = fopen(name, "r");
      if (!f)
        {
          puts("Error! The file doesn't exist!\n");
          return (1);
        }
      else
        {
          int c = 0;
	  			while ((c = getc(f)) != EOF)
	    			{
              putchar(c);
            }
	  				putchar('\n');
        }
      return (0);
    }
  puts("What do you want to add to list?");
  char ls[256][256] = {0};
  getchar();
  for (int i = 0; i < 256; i++)
    {
	    printf("> ");
	    fgets(ls[i], 255, stdin);
	    if (strcmp(ls[i], "EOF\n") == 0)
	      {
	        ls[i][0] = '\0';
	        ls[i][1] = '\0';
	        ls[i][2] = '\0';
	        break;
	      }
    }
  printf("Name your list:\t");
  char name[256] = {0};
  fgets(name, 255, stdin);
  deleten(name);
  FILE *f = fopen(name, "w");
  for (int i = 0; i < 255; i++)
    {
      fputs(ls[i], f);
    }
  fclose(f);
  return (0);
}

