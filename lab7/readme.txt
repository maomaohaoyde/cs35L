At first, I tried to store the pixel colors in a dynamically allocated 2D array
. But it turned out to always have segmentation fault. Then I used dynamically
allocated 1D array. It still does not work. At last I used a 1D array and final
ly figured it out.

My implementation can significantly reduce the painting time of the picture with more than one thread. It takes only half time to run with two threads and 30%
of time with four threads. Running with eight threads, the picture can be paint
ed in about 10 seconds (about 20% of time using one thread.)
