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
Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Verbose (0/1)
3 | Number of dimensions
4 | Epsilon
5 | Maximum number of iterations
6 | Simplex scale
7 | Adaptive
8 | Initialization (0 for coordinates, 1 for range)

If Initialization = 0
Parameter | Meaning
----------|-----------
9 | Lower limit
10 | Upper limit

If Initialization = 1
Parameter | Meaning
----------|-----------
9+ | Coordinates

Example
```
./nm-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```
## Multidirectional Search
Parameter | Meaning
----------|-----------
1 | Display precision (3..36)
2 | Verbose (0/1)
3 | Number of dimensions
4 | Epsilon
5 | Maximum number of iterations
6 | Simplex scale
7 | Initialization (0 for coordinates, 1 for range)

If Initialization = 0
Parameter | Meaning
----------|-----------
8 | Lower limit
9 | Upper limit

If Initialization = 1
Parameter | Meaning
----------|-----------
8+ | Coordinates

Example
```
./mds-ackley-std 3 0 3 1.0e-6 10000 1.0 0 1 -10 10
```
