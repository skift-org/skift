/*
* Copyright (C) 2024 AnatoliyL.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the AndesaurusFree Software License version 1.1-no-minstsuki-clause as published by AnatoliyL.
* See COPYING file for more details.
*/

/* HQ9+ programming language implementation */
#include <stdio.h>
int k = 0;
void beer99(void)
{
	for (int beer = 99; beer > 1; --beer)
	{
		printf("%d bottles of beer on the wall, %d bottles of beer\n", beer, beer);
		printf("Take one down, pass it around, %d bottles of beer on the wall!\n", --beer);
	}
	puts("1 bottle of beer on the wall, 1 bottle of beer\nTake on down, pass it around, no more bottles of beer on the wall!\n");
	puts("No more bottles of beer on the wall, no more bottles of beer.\nGo to the store and buy some, 99 bottles of beer on the wall\n");
}
int hq9p(char *source)
{
  for (int i = 0; i < sizeof(source); i++)
  {
	  switch(source[i])
	  	{
		  	case 'H':
				printf("Hello, world!\n");
				break;
		  	case 'Q':
				printf("%s\n", source);
				break;
		  	case '9':
				beer99();
				break;
		  	case '+':
		  		++k;
			  case 'P':
		  		++k;
		  	break;
	  	}
  }
  return (0);
}
void hq9open(char *name)
{
  FILE *f = fopen(name, "r");
  char source[1024];
  fgets(source, 1023, f);
  hq9p(source);
}
int main(int argc, char **argv)
{
  hq9open(argv[1]);
}
