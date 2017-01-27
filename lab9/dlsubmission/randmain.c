//
//  main.c
//  lab9
//
//  Created by Wang Jingtao on 5/31/15.
//  Copyright (c) 2015 Wang Jingtao. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <immintrin.h>
#include <dlfcn.h>
#include "randcpuid.h"
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
	perror (argv[1]);
      else
	valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  unsigned long long (*rand64) (void);
  void *lib;
  char* err;
  if (rdrand_supported ())
    {
      lib = dlopen("./randlibhw.so", RTLD_NOW);
      if (!lib) {
	printf("failed to open randlibhw.so: %s \n", dlerror());
	exit(1);
      }
      rand64 = dlsym(lib, "rand64");
      err = dlerror(); /*check for errors and copy error message*/
      if (err) {
	printf("failed to locate rand64(): %s \n", err);
	exit(1);
      }


    }
  else
    {
      lib = dlopen("./randlibsw.so", RTLD_NOW);
      if (!lib) {
	printf("failed to open randlibsw.so: %s \n", dlerror());
	exit(1);
      }
      rand64 = dlsym(lib, "rand64");
      err = dlerror(); /*check for errors and copy error message*/
      if (err) {
	printf("failed to locate rand64(): %s \n", err);
	exit(1);
      }
    }

  int wordsize = sizeof rand64 ();
  int output_errno = 0;

      do
	{
	  unsigned long long x = rand64 ();
	  size_t outbytes = nbytes < wordsize ? nbytes : wordsize;
	  if (fwrite (&x, 1, outbytes, stdout) != outbytes)
	    {
	      output_errno = errno;
	      break;
	    }
	  nbytes -= outbytes;
	}
      while (0 < nbytes);

      if (fclose (stdout) != 0)
	output_errno = errno;

      if (output_errno)
	{
	  errno = output_errno;
	  perror ("output");
	  return 1;
	}
      dlclose(lib);
      return 0;
}
