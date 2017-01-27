//
//  randlibsw.c
//  lab9
//
//  Created by Wang Jingtao on 5/31/15.
//  Copyright (c) 2015 Wang Jingtao. All rights reserved.
//

#include "randlib.h"
#include <stdio.h>
#include <stdlib.h>
static FILE *urandstream;
static void
__attribute__ ((__constructor__))
software_rand64_init (void)
{
    urandstream = fopen ("/dev/urandom", "r");
    if (! urandstream)
        abort ();
}
extern unsigned long long
rand64 (void)
{
    unsigned long long int x;
    if (fread (&x, sizeof x, 1, urandstream) != 1)
        abort ();
    return x;
}

static void
__attribute__ ((__destructor__))
software_rand64_fini (void)
{
    fclose (urandstream);
}
