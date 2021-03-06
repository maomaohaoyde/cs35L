//
//  main.c
//  tr2b
//
//  Created by Wang Jingtao on 5/20/15.
//  Copyright (c) 2015 Wang Jingtao. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, const char * argv[])
{
  if (argc!=3)
    {
      fprintf(stderr, "Wrong number of arguments!");
      exit(1);
    }
  const char * old=argv[1];
  const char * new=argv[2];
  if (strlen(old)!=strlen(new))
    {
      fprintf(stderr, "Two arguments must have the same length!");
      exit(1);
    }
  int i;
  for (i=0; i<strlen(old); i++)
    {
      int j;
      for (j=i+1; j<strlen(new); j++)
	{
	  if (old[i]==old[j])
	    {
	      fprintf(stderr, "Characters in the first argument can't repeat!");
	      exit(1);
	    }
	}
    }

  int next=getchar();
  if (ferror(stdin))
    {
      fprintf(stderr, "Fail to read stdin.\n");
      exit(1);
    }
  while (next!=EOF)
    {   int k;
      for (k=0; k<strlen(old); k++)
	{
	  if (next==old[k]) {
	    next=new[k];
	    break;
	  }
	}
      putchar(next);
      if (ferror(stdout))
	{
	  fprintf(stderr, "Fail to write stdout.\n");
	  exit(1);
	}
      next=getchar();
      if (ferror(stdin))
	{
	  fprintf(stderr, "Fail to read stdin.\n");
	  exit(1);
	}
    }
}
