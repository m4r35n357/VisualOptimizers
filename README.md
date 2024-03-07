# Various Gradient-free optimizers

## Pure c99 (plus OpenGL)

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

### c Build (Clang by default)

```
make depclean
make
```
There should be NO errors or warnings.

# Usage

## Nelder-Mead

Nelder, John A.; R. Mead (1965). "A simplex method for function minimization". Computer Journal. 7 (4): 308–313. doi:10.1093/comjnl/7.4.308.

This code was originally based on a [project](https://github.com/matteotiziano/nelder-mead) by Matteo Maggioni in 2023.

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Maximum error for convergence
5 | Maximum number of iterations
6 | Simplex scale
7 | Adaptive
8 | Initialization (0 for explicit coordinates, 1 for random in range)

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
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
./nm-ackley-gl 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```

## Multi-directional Search

See https://www.osti.gov/biblio/5827867 (incidentally, osti.gov have got their pronouns in a twist in a rather surprising way, see https://en.wikipedia.org/wiki/Virginia_Torczon)

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Maximum error for convergence
5 | Maximum number of iterations
6 | Simplex scale
7 | Initialization (0 for explicit coordinates, 1 for random in range)

If Initialization = 0
Parameter | Meaning
----------|-----------
8+ | Coordinate list

If Initialization = 1
Parameter | Meaning
----------|-----------
8 | Lower limit
9 | Upper limit

Examples
```
./mds-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
./mds-ackley-gl 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```

## Whale Optimization

Mirjalili, Seyedali, and Andrew Lewis. "The whale optimization algorithm." Advances in Engineering Software 95 (2016): 51-67.

https://uk.mathworks.com/matlabcentral/fileexchange/55667-the-whale-optimization-algorithm

This is a _tweaked_ version of the algorithm as published:
* "Prey" is explicitly excluded from the update code
* Updated "prey" is calculated and used immediately rather than waiting for each iteration to complete
* Out-of-range agents are randomly "teleported" instead of "clipping" them at the limit
* I have attempted to set the algorithm variable "b" to something useful

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Lower limit
7 | Upper limit

Examples
```
./whale-ackley-std 3 0 3 50 100 -5 5
./whale-ackley-gl 3 0 3 50 100 -5 5
```

## Spiral Optimization

See https://en.wikipedia.org/wiki/Spiral_optimization_algorithm

There is experimental code for repeatedly restarting the algorithm until the result stops changing.
Un-comment the commented line in spiral.c and rebuild to try it.

Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Floating point format (0 for fixed, 1 for exponential)
3 | Number of dimensions
4 | Number of search agents
5 | Number of iterations
6 | Spiral "rule" (0 for "periodic descent", 1 for "convergence")
7 | Lower limit
8 | Upper limit

Examples
```
./spiral-ackley-std 3 0 3 50 100 0 -5 5
./spiral-ackley-gl 3 0 3 50 100 0 -5 5
```
## OpenGL Keyboard Controls
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
'C' 'c' | show centroid for Nelder-Mead
'G' 'g' | increase ball size
'B' 'b' | decrease ball size
'S' 's' | iterate
'F' 'f' | toggle fullscreen mode
'H' 'h' | toggle OSD text
'Q' 'q' ESC | exit

