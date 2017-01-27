//
//  randlib.c
//  lab9
//
//  Created by Wang Jingtao on 5/31/15.
//  Copyright (c) 2015 Wang Jingtao. All rights reserved.
//

#include "randlib.h"
#include <immintrin.h>
inline extern unsigned long long
rand64 (void)
{
    unsigned long long int x;
    while (! _rdrand64_step (&x))
        continue;
    return x;
}
