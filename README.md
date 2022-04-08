Using AVX intrinsics functions to optimize calculating of Mandelbrot set
========================================================================

Testing two versions
--------------------
This repository includes two branches both containing differrent versions of programm, that calculates Mandelbrot and shows it using SFML.
'noopt' branch contains version without any optimizations
'main' branch contains version, optimized using AVX Intel intrinsics functions.

Note: in 'double' branch there is a uncompleted version of the same program, but with using 'double' type for calculating Mandelbrot set
In 'main' version floats are used

Features
--------
src/mandelbrot/mandelbrot_conf.h contains configuration definition:
<code> #define NO_DRAWING </code>

This definition turns off drawing Mandelbrot set in window, so that it doesn't add an error to the measured computation time
Next step we will compare FPS of two versions

Comparing
---------
Version | No optimizations | AVX optimizations with float |
--- | --- |
FPS | 301 | 283 | 

