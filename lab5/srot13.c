#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int rot13cmp(const void *a, const void *b)
{
  const char * first = *((char const **)a);
  const char * second = *((char const **)b);

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

int main()
{
  char ** input;
  int m_row=100,m_col=1000;
  input= (char **)malloc(m_row*sizeof(char*));
  if (input==NULL)
    {
      fprintf(stderr, "Fail to allocate memory.\n");
      exit(1);
    }
  int i=0, j=0;
  char next = getchar(),prev ='\0';


  while (next!=EOF)
    {
      if (j==0)
	{
	  if (i>=m_row)
	    {
	      m_row+=100;
	      char** temp = input;
	      input = (char**)realloc((void*)temp, m_row*sizeof(char*));
	    }
	  if (input==NULL)
	    {
	      fprintf(stderr, "Fail to allocate memory.\n");
	      exit(1);
	    }
	  m_col=1000;
	  input[i]=(char*)malloc(m_col);
	  if (input[i]==NULL)
	    {
	      fprintf(stderr, "Fail to allocate memory.\n");
	      exit(1);
	    }
	}

      if (j>=m_col)
	{
	  m_col+=1000;
	  char* temp = input[i];
	  input[i]=(char*)realloc((void*)temp, m_col);
	  if (input[i]==NULL)
	    {
	      fprintf(stderr, "Fail to allocate memory.\n");
	      exit(1);
	    }
	}

      input[i][j]=next;
      j++;
      if (input[i][j-1]=='\n')
	{
	  i++;
	  j=0;
	}
      prev=next;
      next=getchar();
    }
  if (ferror(stdin))
    {
      fprintf(stderr, "Fail to read stdin.\n");
      exit(1);
    }
  if (prev != '\n')
    {
      if (j == 0)
	{
	  if (i >= m_row)
	    {
	      m_row+=100;
	      char** temp = input;
	      input = (char**)realloc((void*)temp,m_row*sizeof(char*));
	      if (input == NULL)
		{
		  fprintf(stderr, "Fail to allocate memory.\n");
		  exit(1);
		}
	    }


	  m_col=1000;
	  input[i] = (char*)malloc(m_col);
	  if (input[i] == NULL)
	    {
	      fprintf(stderr, "Fail to allocate memory.\n");
	      exit(1);
	    }
	}

      if (j >= m_col)
	{
	  m_col+=1000;
	  char* temp = input[i];
	  input[i] = (char*)realloc((void*)temp, m_col);
	  if (input[i] == NULL)
	    {
	      fprintf(stderr, "Fail to allocate memory.\n");
	      exit(1);
	    }
	}

      input[i][j] = '\n';
      i++;
      j++;
    }

  qsort((void*)input,i,sizeof(char*),rot13cmp);
  int n=0;

  while (n<i)
    {
      int m=0;
      while (input[n][m]!='\n')
	{
	  printf("%c",input[n][m]);
	  m++;
	  if (ferror(stdout))
	    {
	      fprintf(stderr, "Fail to write in stdout.\n");
	      exit(1);
	    }
	}
      printf("%c",input[n][m]);
      n++;
    }
  free(input);
  exit(0);
}
//new lines + something
//empty file
//wrong number of arguments
