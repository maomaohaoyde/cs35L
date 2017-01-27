#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
long comparisons;
int rot13cmp(const void *a, const void *b)
{
  const char * first = *((char const **)a);
  const char * second = *((char const **)b);
  comparisons++;
  int i=0;
  while (1)
    {
      if (first[i]=='\n' && second[i]=='\n')
	{
	  return 0;
	}
      if (first[i] == '\n')
	{
	  return -1;
	}
      if (second[i]=='\n') {
	return 1;
      }
      if (first[i]!=second[i] )
	{
	  if (first[i]>='A' && first[i]<='M' && second[i]>='N' && second[i]<='Z')
	    return 1;
	  if (first[i]>='a' && first[i]<='m' && second[i]>='n' && second[i]<='z')
	    return 1;
	  if (first[i]>='N' && first[i]<='Z' && second[i]>='A' && second[i]<='M')
	    return -1;
	  if (first[i]>='n' && first[i]<='z' && second[i]>='a' && second[i]<='m')
	    return -1;
	  return first[i]-second[i];
	}
      i++;
    }
}

int main(int argc, const char * argv[]) {


  if (argc!=1)
    {
      fprintf(stderr, "Wrong number of arguments!");
      exit(1);
    }


  struct stat fileStat;
  if (fstat(0, &fileStat)<0)
    {
      fprintf(stderr, "Unable to get the information of the file!");
      exit(1);
    }


  char* input;
  long dynNum=1000;
  if (S_ISREG(fileStat.st_mode))
    {
      input=(char*)malloc(fileStat.st_size);
      dynNum=fileStat.st_size;
    }
  else
    input=(char*)malloc(dynNum);


  int i=0;
  char next;
  ssize_t status=read(0,&next , 1);

  while (status!=0)
    {
      if (i==dynNum) {
	dynNum+=1000;
	char* temp = input;
	input = (char*)realloc((void*)temp, dynNum);
      }
      input[i]=next;
      i++;
      status=read(0,&next , 1);
    }


  char ** converted;
  int numOfRows=100;
  converted=(char**)malloc(numOfRows*sizeof(char*));
  if (converted==NULL)
    {
      fprintf(stderr, "Fail to allocate memory.\n");
      exit(1);
    }
  int curNumOfRow=0,curNumOfCol=0,numOfCol=100,j;
  converted[curNumOfRow]=(char*)malloc(numOfCol*sizeof(char));

  for (j=0; j<i; j++)
    {
      if (curNumOfRow==numOfRows)
	{
	  numOfRows+=100;
	  char** temp=converted;
	  converted=(char**)realloc((void*)temp, numOfRows*sizeof(char*));
	}

      if (converted==NULL)
	{
	  fprintf(stderr, "Fail to allocate memory.\n");
	  exit(1);
	}
      converted[curNumOfRow][curNumOfCol]=input[j];
      curNumOfCol++;
      if (curNumOfCol==numOfCol)
	{
	  numOfCol+=100;
	  char* temp=converted[curNumOfRow];
	  converted[curNumOfRow]=(char*)realloc((void*)temp, numOfCol*sizeof(char));
	}
      if (input[j]=='\n')
	{   curNumOfRow++;
	  converted[curNumOfRow]=(char*)malloc(numOfCol*sizeof(char));
	  curNumOfCol=0;
	}
    }


  if (input[j-1]!='\n')
    {
      converted[curNumOfRow][curNumOfCol]='\n';
      curNumOfRow++;
      curNumOfCol++;
    }
  comparisons=0;
  qsort((void*)converted,curNumOfRow,sizeof(char*),rot13cmp);
  int n=0;

  while (n<curNumOfRow)
    {
      int m=0;
      while (converted[n][m]!='\n')
	{
	  write(1, &converted[n][m], 1);
	  m++;
	  if (ferror(stdout))
	    {
	      fprintf(stderr, "Fail to write in stdout.\n");
	      exit(1);
	    }
	}
      if (m!=0)
	write(1, &converted[n][m], 1);
      n++;
    }
  free(input);
  fprintf(stderr, "Number of comparisons: %ld \n", comparisons);
  exit(0);

}
