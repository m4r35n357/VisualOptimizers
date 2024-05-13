# Simple Gradient-free optimizers

Here are some basic "black-box" optimizers, for experimentation and visualization.
If you are unsure what the ["Curse of Dimensionality"](https://en.wikipedia.org/wiki/Curse_of_dimensionality) means, or ["No Free Lunch"](https://en.wikipedia.org/wiki/No_free_lunch_theorem), this project might help.

There are currently four candidate algorithms included, each with an interactive OpenGL visualizer:

* Nelder-Mead - features a "multi-run" bulk mode for global optimization
* Optimization by Cut (including the "standard" version, my simplified version, and a random version!)

Suitable for global optimization in up to 16 dimensions for the cut algorithm, with scenarios up to 64 dimensions for "bulk-mode" Nelder-Mead.

## Pure c99 (plus optional 3D OpenGL visualization)

The code is concise and efficient, and produces tiny executables.
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
Also, there is a pre-commit script for use with git, which rebuilds everything and runs some simple scenarios.
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
make test-multi-3d
```
shows basic program output (with stdout suppressed!) and can be used to get OpenGL commands by cut & paste - just change std to gl in the program names.
Bulk mode Nelder-Mead, and cut.
The "multi" target runs stats.
```
make CCC=gcc test-8d
make CCC=gcc test-multi-8d
```
shows bulk mode Nelder-Mead, and cut.
The "multi" target runs stats.
Fewer models than 3D.

```
make CCC=gcc test-16d
make CCC=gcc test-multi-16d
```
shows bulk mode Nelder-Mead, and cut.
The "multi" target runs stats.
Fewer models than 8D.
The cut optimizer is still hanging on at 16D, but the "curse of dimensionality" means that they will need an eye-watering number of iterations to work at 32D.

```
make CCC=gcc test-32d
make CCC=gcc test-multi-32d
```
shows single-run Nelder-Mead.
The "multi" target runs stats.
Fewer models than 16D.
```
make CCC=gcc test-64d
make CCC=gcc test-multi-64d
```
shows single-run Nelder-Mead.
The "multi" target runs stats.

# Usage

The make tests above show summary output for models of interest.
All programs and scripts output their command arguments for easy cut & paste (magenta for scripts, cyan for binaries.
The commands and their arguments are detailed below.

## Nelder-Mead

Nelder, John A.; R. Mead (1965). "A simplex method for function minimization". Computer Journal. 7 (4): 308–313. doi:10.1093/comjnl/7.4.308.

This code was originally based on a [project](https://github.com/matteotiziano/nelder-mead) by Matteo Maggioni in 2023.
It has since been extensively rewritten, now uses regular simplexes, and has a "multi-run" (or "bulk") mode for use as a _global_ optimizer.

The regular vertex coordinates are generated using an algorithm which is described (open access) [here](https://link.springer.com/article/10.1007/s11590-022-01953-y).
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
./nm-sphere-std 3 fixed 3 1.0e-6 2627 10.0 non-adaptive bulk -10 10 >/dev/null
./nm-sphere-gl 3 fixed 3 1.0e-6 2627 10.0 non-adaptive random -10 10 
```
When parameter 8 is set to a non-zero value, the algorithm is run several times until the cumulative number of _evaluations_ exceeds this value (budget).
This enables meaningful comparisons with the other methods.

The OpenGL visualization shows two "dual" regular initial simplexes.
For each one, the best vertex is green, and the worst is red.
The remaining vertices are coloured either cyan (default simplex) or gold (dual simplex), correspondiing to the OSD text colour.

##  Optimization by Cut & Random Optimization

Algorithm described [here](https://arxiv.org/abs/2207.05953v1).

There is one simplification I have made; instead of "clamping" the new box to be entirely within the previous one, I allow the box to be centered around the new minimum, giving a degree of movement to the box that would otherwise be absent.
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
The OpenGL visualization shows both cut and random algorithms regardless of parameter 6.
Random mode is represented by green particles, with the best point marked in red.
Cut mode (my modification, which allows the box a degree of movement) is represented by cyan particles in a magenta box, with the best point marked in magenta.

## The Models

These are the most "important", in decreasing order

Model | Justification
----------|-----------
sphere | unimodal "hello world" of optimization models.  No excuse for this not working for any optimizer!
trid | slightly more involved, but still unimodal and well behaved
rosenbrock | unimodal, but tests ability to cope with contrasting directional slopes
easom | unimodal "needle in a haystack".  Also tests machine precision!
treacle | unimodal but non-convex
dixon-price | multiple global maxima, and one _very_ attractive local minimum (for dimensions > 3)
levy | multimodal but not too pathological

The other models work only at lower dimensions, or do not give clear enough data to draw any useful conclusions.

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

Parameters 3 and 4 are multiplied within the script to give a "budget" of evaluations for Nelder-Mead.

Example
```
make clean
make CCC=gcc
./solve-model easom 8 256 100 -15 15
```

To get a clearer understanding of the run-by-run variation in performance of the various integrators, use the "stats" script below.

## "stats" script

Runs a single algorithm multiple times agains a target value for a single model.
For each run, the output is green if the result is below a user-supplied threshold, and red otherwise.

Parameter | Meaning
----------|-----------
1 | Number of runs
2 | Threshold
3+ | optimizer command

Examples
```
make clean
make CCC=gcc
./stats 100 0.001 ./nm-dixon-price-std 3 fixed 8 1.0e-6 25756 10.0 adaptive bulk -10 10
./stats 100 0.03 ./cut-treacle-std 3 fixed 8 256 100 unclamped 0 10
```
For 8D or higher, using one of the faster "CCC=" make options above is _highly_ recommended!

## "Global" Optimization

A common feature of particle or "swarm-based" methods is an _exploration_ phase followed by a _refinement_ phase (in practice the transition is a gradual process).
Refinement is _not_ the same as convergence; you get what you are given after a specified number of iterations!
Another way of looking at it is that the initial search domain is shrunk by a large factor, concentrating the search agents into a relatively small volume around a _potential_ minimum.
In any case, this refinement stage makes it harder to jump out of a stubborn local minimum.

I have adapted the Nelder-Mead method to do a series of random runs, while keeping the best result, and accounting for total number of function evaluations.
The size of the initial simplex is set to a large value, and the "adaptive" setting is used for 8D and above; this setup works surprisingly well as a global optimizer!

The random exploration is not limited by any refinement process so global minima are always accessible, even if not actually reached within the set limits.

The comparisons are _roughly_ equivalent in that the number of Nelder-Mead evaluations is set roughly to (agents) x (iterations), which is a good approximation to the number of evaluations used by the "swarm-based" optimizers.
Do your own experiments!

## "multi-stats" script

Runs the "stats" script many times for all algorithms (except random) for a single model, to help even out the typically large fluctuations in results from run to run, and get a more realistic view on comparative performance of the algorithms.

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

Parameters 5 and 6 are multiplied within the script to give a "budget" of evaluations for Nelder-Mead and random.

Examples
```
make clean
make CCC=gcc
./multi-stats 100 0.001 sphere 8 256 1000 -10 10
./multi-stats 100 0.03 treacle 8 256 1000 -10 10
```

Another way to do this is using make, which invokes the script for the most "important" models.
```
make clean
make CCC=gcc test-multi-3d
make CCC=gcc test-multi-8d
```

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
Observe the way coordinates above the third are projected into the 3D space (ignored!).

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
