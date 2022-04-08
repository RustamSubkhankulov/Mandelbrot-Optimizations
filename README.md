Using AVX intrinsics functions to optimize calculating of Mandelbrot set
========================================================================

Testing two versions
--------------------
This repository includes two branches both containing differrent versions of program, that calculates Mandelbrot and shows it using SFML.
'noopt' branch contains version without any optimizations
'main' branch contains version, optimized using AVX Intel intrinsics functions.

Note: in 'double' branch there is a uncompleted version of the same program, but with using 'double' type for calculating Mandelbrot set
In 'main' version floats are used

Note: 'noopt' version compiles with optimizations flar -O2, as version with optimization for fair compare

Features
--------
src/mandelbrot/mandelbrot_conf.h contains configuration definition:
<code> #define NO_DRAWING </code>

This definition turns off drawing Mandelbrot set in window, so that it doesn't add an error to the measured computation time
Next step we will compare FPS of two versions

Controls
--------

One can use controls to move picture of Mandelbrot set and enjoy its beauty

1. Arrows - moving picture up/down/left/right
2. PgUp/PgDown - ZoomIn / ZoomOut

Comparing
---------

| no optimization  | 8.1  FPS       | 
| AVX with floats  | 50.8 FPS       |

Conclusion
----------

Increase in performance estimately 6,3 times. ons
Although AVX2 instructions performs calculations with 8 values at one time, increase is not 8 times. This probably causes by other instructions in algorithm, for example, memory access
