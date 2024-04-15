# Simple Gradient-free optimizers

Currently three candidates:

* Nelder-Mead - multi-run mode for global optimization
* Spiral Optimization (using "dual" rotations in OpenGL) - two algorithm strategies
* Optimization by Cut (both strategies in OpenGL) - two algorithm strategies

Presented for application in local or global optimization in up to 16 dimensions.
Additionally there is a "random" optimizer (with no OpenGL visualization) to use as a base-line.

## Pure c99 (plus optional 3D OpenGL visualization)

The programs can be built either with Clang or GCC.
All console programs are written to and depend _only_ on the c99 standard and library (strictly speaking, the WG14/N1256 _draft_ standard!).
External dependencies (OpenGL, FreeGLUT & GLEW) are only needed for the OpenGL plotters (*-gl).

All simulation floating point operations are executed in _long double_ precision.
This gives a choice of precision and performance on different platforms.

Platform | Long Double Implementation
----------|-----------
x86 / x86-64 | 80 bit hardware float
armhf | 64 bit hardware float
aarch64 | 128 bit _software_ float

These are converted to _float_ for use by OpenGL.

All scripts are in _Posix_ shell syntax.

## Quick Start

### Requirements - Debian/Ubuntu packages
```
sudo apt install bc git build-essential freeglut3-dev glew-utils libglew-dev
```

### Download
```
git clone https://github.com/m4r35n357/GradientFreeOptimizers/single-simplex-ic
cd single-simplex-ic
```

### c Build (Clang with debug by default)

```
make depclean
```
once only, unless you have edited any header files, then either of
```
make
make nogl
```
There should be NO errors or warnings.
The default build requires OpenGL development libraries, and produces the *-gl executables.
The nogl build is pure c99 and produces the *-std executables.
C tags are built automatically, which can make things more comfortable in some editors and IDEs.
Also, there is a pre-commit script for use with git, which rebuilds everything and runs some tests.
To run it:
```
./pre-commit
```
To install it:
```
cp pre-commit .git/hooks
```
Fast executables (using -O3 -fno-math-errno -flto) make a big difference with "bulk" running (e.g. multi-run Nelder-Mead, and particularly the stats script):
```
make CCC=gcc
make CCC=clang
```
See the Makefile for various additional "test" targets.
In particular:
```
make test-3d
```
shows basic program output (with stdout suppressed!) and can be used to get OpenGL commands by cut & paste - just change std to gl in the program names.
Single-run Nelder-Mead, spiral (both strategies), cut (both strategies).
```
make test-8d
```
shows multiple-run Nelder-Mead, spiral (both strategies), and cut (both strategies).
Fewer models.
The spiral optimizers are clearly running out of steam at 8D.

```
make test-16d
```
shows multiple-run Nelder-Mead, and cut (both strategies).
Fewer models.
The cut optimizers are still hanging on at 16D, but the "curse of dimensionality" means that they will need an eye-watering number of iterations to work at 32D.

```
make test-32d
```
shows just multiple-run Nelder-Mead.
Fewer models.

# Usage

## Nelder-Mead

Nelder, John A.; R. Mead (1965). "A simplex method for function minimization". Computer Journal. 7 (4): 308–313. doi:10.1093/comjnl/7.4.308.

This code was originally based on a [project](https://github.com/matteotiziano/nelder-mead) by Matteo Maggioni in 2023.
It has been extensively rewritten, now uses regular simplexes, and has a "multi-run" mode for use as a global optimizer.

The regular vertex coordinates are generated using an algorithm which is described (open access) [here](https://link.springer.com/article/10.1007/s11590-022-01953-y).
The adaptive algorithm is described (open access) [here](https://www.researchgate.net/publication/225691623_Implementing_the_Nelder-Mead_simplex_algorithm_with_adaptive_parameters).

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Maximum error for convergence
5 | Maximum number of iterations
6 | Initial simplex scale
7 | Adaptive (0 for no, 1 for yes)
8 | Initialization (0 for explicit coordinates, 1 or more for number of random "runs") - ignored for GL

If Initialization = 0
Parameter | Meaning
----------|-----------
9+ | Coordinate list

If Initialization = 1
Parameter | Meaning
----------|-----------
9 | Lower limit
10 | Upper limit

Examples
```
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10 >/dev/null
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 100 -10 10 >/dev/null
./nm-ackley-gl 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```
The OpenGL visualization shows two "dual" regular initial simplexes.

## Spiral Optimization

Based on the algorithm described [here](https://en.wikipedia.org/wiki/Spiral_optimization_algorithm).
Note that current versions of the algorithm use a fixed rotation angle of PI/2; versions using variable theta are now obsolete.
The code supports both modes of operation described in that link: "Periodic Descent Direction Setting" and "Convergence Setting".

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Algorithm mode (0 for "periodic descent", 1 for "convergence")
7 | Lower limit
8 | Upper limit

Examples
```
./spiral-ackley-std 3 0 3 64 100 0 -5 5
./spiral-ackley-std 3 0 3 64 100 1 -5 5
./spiral-ackley-gl 3 0 3 64 100 0 -5 5
```
The OpenGL visualization shows the two alternative senses of rotation.

##  Optimization by Cut

Algorithm described [here](https://arxiv.org/abs/2207.05953v1).

Use the OpenGL visualizations to see how this works.

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Algorithm mode (0 for "un-clamped", 1 for "clamped"), ignored in OpenGL
7 | Lower limit
8 | Upper limit

Examples
```
./cut-ackley-std 3 0 3 64 100 0 -5 5
./cut-ackley-std 3 0 3 64 100 1 -5 5
./cut-ackley-gl 3 0 3 64 100 0 -5 5
```
The OpenGL visualization shows both "clamped" and "un-clamped" variants regardless of parameter 6.

##  Random Optimization

No OpenGL visualization provided

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of iterations
5 | Show progress (0 for no, 1 for yes)
6 | Lower limit
7 | Upper limit

Examples
```
./rnd-ackley-std 3 0 3 64000 0 -5 5
./rnd-ackley-std 3 0 3 64000 1 -5 5
```

## "solve-model" script

Run a model against all integrators, including random

Parameter | Meaning
----------|-----------
1 | Model name
2 | Number of dimensions
3 | Number of search agents
4 | Number of iterations
5 | Number of Nelder-Mead runs
6 | Lower limit
7 | Upper limit

Example
```
./solve-model sphere 8 200 500 1 -10 10
```

To get a better idea of the run-by-run variation in performance of spiral and cut integrators, use the "stats" script below.

## "stats" script

Runs spiral or cut algorithm multiple times agains a target value.
For each run, the output is green if the result is below the threshold, and red otherwise.

Parameter | Meaning
----------|-----------
1 | Number of runs
2 | Threshold
3+ | spiral or cut "std" command

Examples
```
./stats 100 -117 ./spiral-st-std 3 0 3 30 100 1 -5 5
./stats 100 -117 ./cut-st-std 3 0 3 30 100 -5 5
```
For 8D or higher, one of the faster "CCC=" make options above is recommended.

## "Global" Optimization

A common feature of "global" methods is an _exploration_ phase followed by a _refinement_ phase (in practice the transition is a gradual process).
Refinement is not the same as convergence; you get what you are given after a specified number of iterations!
Perhaps "settling" would be a better description.
In any case, this refinement stage makes it harder to jump out of a stubborn local minimum.

I have adapted the Nelder-Mead method to do a series of random runs, while keeping the best result. and accounting for total number of iterations and function evaluations.
This works surprisingly well!
The random exploration is not limited by any refinement process so global minima are always accessible, even if not actually reached within the set limits.

The [Dixon-Price function](https://www.sfu.ca/~ssurjano/dixonpr.html) is a very good example of a function with a stubborn local minimum (and _all_ minima at non-zero coordinates) that gets harder to escape as dimension increases.
To see the problem, try these commands, and then experiment with changing the number of agents, iterations etc.:
```
./stats 100 0.001 ./spiral-dixon-price-std 3 0 8 1000 1000 0 -10 10
./stats 100 0.001 ./spiral-dixon-price-std 3 0 8 1000 1000 1 -10 10
./nm-dixon-price-std 3 0 8 1.0e-6 100000 20.0 1 100 -10 10 >/dev/null
```
The [Styblinski-Tang function](https://www.sfu.ca/~ssurjano/stybtang.html) is also troublesome, but less "pathlogical".
```
./stats 100 -313.0 ./spiral-st-std 3 0 8 100 1000 0 -5 5
./stats 100 -313.0 ./spiral-st-std 3 0 8 100 1000 1 -5 5
./nm-st-std 3 0 8 1.0e-6 100000 10.0 1 100 -5 5 >/dev/null
```
Here are some 16-D starting points:
```
./nm-dixon-price-std 3 0 16 1.0e-6 1000000 10.0 1 1000 -10 10 >/dev/null
```
```
./nm-st-std 3 0 16 1.0e-6 1000000 10.0 1 1000 -5 5 >/dev/null
```
Of course this is comparing apples to oranges, with just two functions, and the results are not always clear-cut (and vary with each run), but total iterations and function evaluations are shown explicitly in each case.
Do your own experiments!

## "multi-stats" script

Runs the "stats" script many times to help even out fluctuations in results from run to run.

Parameter | Meaning
----------|-----------
1 | number of runs
2 | threshold
3 | model name
4 | number of dimensions
5 | number of search agents
6 | number of iterations per run
7 | range min
8 | range max

The easiest way to do this is using make, which invokes the script for the most "important" models (note: for the output below I have edited the Makefile to do 1000 runs per model instead of 100!):
```
make clean
make CCC=gcc test-multi-8d
```
For 1000 runs expect it to take up to an hour to complete.
This is an edited summary of the output:
```
                             Command                         spiral     spiral         cut         cut
                                                             (descent)  (convergence)  (unclamped) (clamped)

Unimodal

     [ ./multi-stats 1000 0.001 sphere 8 256 1000 -10 10 ]     183        862           1000        1000

       [ ./multi-stats 1000 0.001 trid 8 256 1000 -25 25 ]    1000       1000           1000        1000

     [ ./multi-stats 1000 -0.999 easom 8 256 1000 -15 15 ]     390        903           1000         999

   [ ./multi-stats 1000 0.1 rosenbrock 8 256 1000 -10 10 ]       1         31            854           0

      [ ./multi-stats 1000 0.1 treacle 8 256 1000 -10 10 ]       0         31            930         768

Multi-modal

         [ ./multi-stats 1000 -313.0 st 8 256 1000 -5 10 ]     110         65            288         396

[ ./multi-stats 1000 0.001 dixon-price 8 256 1000 -10 10 ]     194        263            239          44

       [ ./multi-stats 1000 0.001 levy 8 256 1000 -10 10 ]      13         48            985         833

 [ ./multi-stats 1000 -7.5 michalewicz 8 256 1000 0 3.14 ]     311        242            324         719
```

## OpenGL Visualizations

To get a list of OpenGL command examples, use one of the commands below:
```
grep -- '-std 3 0 3 ' Makefile | sed 's/^.*@//' | sed 's/-std/-gl/' | sed 's/ >.*$//' | sort
grep -- '-std 3 0 3 ' Makefile | sed 's/^.*@//' | sed 's/-std/-gl/' | sed 's/ >.*$//' | grep nm
grep -- '-std 3 0 3 ' Makefile | sed 's/^.*@//' | sed 's/-std/-gl/' | sed 's/ >.*$//' | grep spiral
grep -- '-std 3 0 3 ' Makefile | sed 's/^.*@//' | sed 's/-std/-gl/' | sed 's/ >.*$//' | grep cut
```

## OpenGL Keyboard Controls

The global minimum is no longer displayed by default, use the 'm' key to show it at any time.
For a more immersive "black-box" exerience, leave it until the end!

Key | Action
----------|-----------
UP arrow | increase view elevation
DOWN arrow | decrease view elevation
LEFT arrow | increase view azimuth
RIGHT arrow | decrease view azimuth
HOME | zoom in (fine)
END | zoom out (fine)
PAGE UP | zoom in (coarse)
PAGE DOWN | zoom out (coarse)
'C' 'c' | show/hide centroid (for Nelder-Mead only)
'M' 'm' | show/hide global minimum
'G' 'g' | increase ball size
'B' 'b' | decrease ball size
'S' 's' | iterate
'F' 'f' | toggle fullscreen mode
'H' 'h' | toggle OSD text
'Q' 'q' ESC | exit
