# Simple Visual Gradient-free optimizers

Here are some basic global optimizers, for experimentation and learning through visualization.
If you are unsure what the ["Curse of Dimensionality"](https://en.wikipedia.org/wiki/Curse_of_dimensionality) means, or ["No Free Lunch"](https://en.wikipedia.org/wiki/No_free_lunch_theorem), this project might help.

There are currently _four_ candidate algorithms included (two good, two not so good!), each with an interactive OpenGL visualizer:

* Nelder-Mead - features a "multi-run" bulk mode for global optimization, using an adaptive algorithm, and sensible simplex initialization
* Optimization by Cut (including the published algorithm, my simplified & improved version, and a completely random optimizer!)

The methods are suitable for global optimization in up to 16 dimensions with the cut algorithms, and scenarios up to 64 dimensions for "bulk-mode" Nelder-Mead.

Now includes "real-world" RF filter synthesis examples, with electrical component values as the parameter space (variables).

Firstly there is a multi-order Butterworth model, which is optimized against the theoretical maximally-flat frequency response (an explicit, closed-form solution).
Against this ideal, the error can theoretically be optimized down to zero (subject to numerical errors at higher filter order).

Secondly, there are two low-pass notch designs, to be optimized against a regular filter specification.
In this case, there is _no explicit solution_, and if the spec is exceeded (the minimum hits zero), there is a _continuum_ of solutions.

## Pure c99 (plus optional 3D OpenGL visualization)

The code is concise (currently < 1400 LOC) and efficient, and produces tiny executables (~ 20-40KB).
The programs can be built with either Clang or GCC.
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
git clone https://github.com/m4r35n357/VisualOptimizers
cd VisualOptimizers
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
Also, there is a pre-commit script for use with git, which rebuilds everything and runs some simple scenarios.
To run it:
```
./pre-commit
```
To install it:
```
cp pre-commit .git/hooks
```
The default make command enables debugging and uses optimization level 0.
It is good for use with IDEs, and is also used by the commit script.
Fast executables (using -O3 -fno-math-errno -flto) can also be built, and make a big difference with "bulk" running (e.g. multi-run Nelder-Mead, and particularly the stats script):
```
make CCC=gcc
make CCC=clang
```
See the Makefile for various additional "test" targets.
In particular:
```
make test-3d
make CCC=gcc test-8d
make CCC=gcc test-16d
make CCC=gcc test-32d
make CCC=gcc test-64d
```
show basic program output (with stdout suppressed!) and can be used to get OpenGL commands by cut & paste - just change std to gl in the program names.

The "multi" target runs stats:
```
make CCC=gcc test-multi-3-16
```
Performs 3D, 8D and 16D stats runs using the Nelder-Mead and (unclamped) cut algorithms.
```
make CCC=gcc test-multi-16-64
```
Performs 16D, 32D, and 64D stats runs using Nelder-Mead only.
Taken together, these two "multi" targets produce a good overall performance summary of the various algorithms and models.
```
make CCC=gcc test-filters
```
Synthesizes RF filter components for various orders of filter.
```
make CCC=gcc test-multi-filters
```
Some stats runs for the Butterworth and "notch" LC filters for various orders.

# Usage

The make tests above show summarized output for models of interest.
All programs and scripts output their command arguments for easy cut & paste (magenta for scripts, cyan for binaries).
The commands and their arguments are detailed below.

## Nelder-Mead

Nelder, John A.; R. Mead (1965). "A simplex method for function minimization". Computer Journal. 7 (4): 308–313. doi:10.1093/comjnl/7.4.308.

This code was originally based on a [project](https://github.com/matteotiziano/nelder-mead) by Matteo Maggioni in 2023.
It has since been extensively rewritten, now uses _regular_ simplexes, and has a "multi-run" (or "bulk") mode for use as a _global_ optimizer.

_Regular_ vertex coordinates are generated using an algorithm which is described (open access) [here](https://link.springer.com/article/10.1007/s11590-022-01953-y).
The adaptive algorithm is described (open access) [here](https://www.researchgate.net/publication/225691623_Implementing_the_Nelder-Mead_simplex_algorithm_with_adaptive_parameters).

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (fixed / exp)
3 | Number of dimensions
4 | Maximum error for convergence
5 | Number of evaluations (maximum per iteration, and also the total in bulk mode)
6 | Initial simplex scale
7 | Adaptive (non-adaptive / adaptive)
8 | Initialization (point / random / bulk) - ignored for GL

If Initialization == point
Parameter | Meaning
----------|-----------
9+ | Coordinates of starting point

If Initialization == random or bulk
Parameter | Meaning
----------|-----------
9 | Lower limit
10 | Upper limit

Examples
```
./nm-trid-std 3 fixed 3 1.0e-6 2627 10.0 non-adaptive bulk -10 10 >/dev/null
./nm-trid-gl 3 fixed 3 1.0e-6 2627 10.0 non-adaptive random -10 10 
```
When parameter 8 is set to a non-zero value, the algorithm is run several times until the cumulative number of _evaluations_ exceeds this value (budget).
This enables meaningful comparisons with the other methods.

The OpenGL visualization shows two "dual" regular initial simplexes.
For each one, the best vertex is green, and the worst is red.
The remaining vertices are coloured either cyan (default simplex) or gold (dual simplex), correspondiing to the OSD text colour.

##  Optimization by Cut & Random Optimization

Algorithm described [here](https://arxiv.org/abs/2207.05953v1).

There is one key modification that I have made; instead of "clamping" the new box to be entirely within the previous one, I allow the box to be centered around the new minimum, giving a degree of movement to the box that would otherwise be absent.
I have found experimentally that this improves success rates in all cases of interest.
I retain the published "clamping" mechanism to confine the box to within the _original_ boundary.

The algorithm has now been updated with a random optimizer, which is identical to the cut algorithms except that the box is never shrunk!

Use the OpenGL visualizations to see how this all works.

Parameter | Meaning
----------|-----------
1 | Display precision (1..36)
2 | Floating point format (fixed / exp)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | *Algorithm mode (unclamped / clamped / random)
7 | Lower limit
8 | Upper limit

* ignored in OpenGL, which displays all modes

Examples
```
./cut-trid-std 3 fixed 3 27 100 unclamped -10 10 >/dev/null
./cut-trid-gl 3 fixed 3 27 100 unclamped -10 10 
```
The OpenGL visualization shows both of the cut as well as the random algorithms regardless of parameter 6.
Clamped mode (the published method) is represented by green particles in a red box, with the best point marked in red.
Unclamped mode (my modification, which allows the box a degree of movement) is represented by cyan particles in a magenta box, with the best point marked in magenta.
Random mode is represented by gold particles, with the best point marked in yellow.

## The Models

These are the most "important", in decreasing order

Model | Justification
----------|-----------
e3, e5 | low-pass notch filter design from specifications (passband ripple, selectivity, stopband loss)
bw | symmetric Butterworth RF low-pass filter design (filter order = 2 * dimension - 1)
trid | slightly more involved than the well-known sphere model, but still unimodal and well behaved
rosenbrock | unimodal, non-convex, tests ability to cope with contrasting directional slopes
easom | unimodal, non-convex, "needle in a haystack".  Also tests machine precision!

### Filters

These commands will provide some cut & paste examples:
```
make CCC=gcc test-filters
make CCC=gcc test-multi-filters
```

## "solve-model" script

Run a single model against all integrators, including random

Parameter | Meaning
----------|-----------
1 | Model name
2 | Number of dimensions
3 | Number of search agents
4 | Number of iterations
5 | Lower limit
6 | Upper limit

Parameters 3 and 4 are multiplied within the script to give a "budget" of evaluations for Nelder-Mead in bulk mode.

Example
```
make clean
make CCC=gcc
./solve-model easom 8 256 100 -15 15
```

To get a clearer understanding of the run-by-run variation in performance of the various integrators, use the "stats" script below.

## "stats" script

Runs a single algorithm multiple times agains a target value for a single model.

Parameter | Meaning
----------|-----------
1 | Number of runs
2 | Threshold
3+ | optimizer command

For each run, the runs/iterations/evaluations counters are green if the result is below the user-supplied threshold, and red otherwise.
Total number of passes is printed at the end, colour coded as:

Colour | Percentage
----------|-----------
green | > 90%
cyan | 70% -> 90%
orange/brown | 20% -> 70%
red | < 20%

Examples
```
make clean
make CCC=gcc
./stats 100 0.001 ./nm-trid-std 3 fixed 8 1.0e-6 25756 10.0 adaptive random -64 64
./stats 100 0.03 ./cut-trid-std 3 fixed 8 256 100 unclamped -64 64
```
For 8D or higher, using one of the faster "CCC=" make options above is _highly_ recommended!

## "Global" Optimization

A common feature of particle-based methods is an _exploration_ phase followed by a _refinement_ phase (in practice the transition is a gradual process).
Refinement is _not_ the same thing as convergence; you get what you are given after a specified number of iterations!
Another way of looking at it is that the initial search domain is shrunk by a large factor, concentrating the search agents into a relatively small volume around a _potential_ minimum.
In any case, this refinement stage makes it harder to jump out of a stubborn local minimum.

I have adapted the Nelder-Mead method with an option to do a series of random runs, while keeping the best result, and accounting for total number of function evaluations.
The size of the initial simplex is set to a large value, and the _adaptive_ setting is used for 8D and above; this setup works rather well as a global optimizer!
This is because the random exploration is not subject to any refinement process so global minima are always potentially accessible, even if not actually reached within the set limits.

The comparisons are _roughly_ equivalent in that the number of Nelder-Mead evaluations is set roughly to (agents) x (iterations), which is a good approximation to the number of evaluations used by the particle-based optimizers.
Do your own experiments!

## "multi-stats" script

Runs the "stats" script multiple times for all algorithms against a single model, to help even out the typically large fluctuations in results from run to run, and get a more realistic view on comparative performance of the algorithms.
Total passes from the stats script are printed for each algorithm.

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

Parameters 5 and 6 are multiplied within the script to give a "budget" of evaluations for Nelder-Mead.

Examples
```
make clean
make CCC=gcc
./multi-stats 100 0.001 trid 8 256 1000 -64 64
```
See the Makefile for other examples.

## Interactive OpenGL Visualizations

To get a list of 3D OpenGL command examples for cut & paste into a terminal, use one of the commands below:
```
make test-3d 2>&1 | grep std | sed 's/^.*\[ //' | sed 's/-std/-gl/' | sed 's/[ ]*\]//' | grep nm
make test-3d 2>&1 | grep std | sed 's/^.*\[ //' | sed 's/-std/-gl/' | sed 's/[ ]*\]//' | grep cut
```
The visualizations also "work" for more than three dimensions, obviously they do not tell the whole story in this case, but can still give some kind of indication of progress.
For example, try this (it will take a litttle longer to run than the 3D case):
```
make test-8d 2>&1 | grep std | sed 's/^.*\[ //' | sed 's/-std/-gl/' | sed 's/[ ]*\]//' | grep cut
```
Coordinates above the third are projected into the 3D space (ignored!).

## OpenGL Keyboard Controls

The normal mode of use is to manually step through the iteration sequence using the "s" key.

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

Examples
```
./nm-trid-gl 3 fixed 3 1.0e-6 10000 10.0 non-adaptive random -10 10 
./cut-trid-gl 3 fixed 3 27 100 unclamped -10 10 
```
