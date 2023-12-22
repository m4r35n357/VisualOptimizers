#!/bin/sh

###
# 
# 
# Usage
# $ ./run.sh -b BIN -p INITIAL_POINT
#       [-x TOL_X] [-f TOL_F] [-i MAX_ITER] [-e MAX_EVAL]
#       [-d DISPLAY_PRECISION] [-v VERBOSE] 
#       [-a ADAPTIVE_SCALING] [-s SIMPLEX_SCALING]
#         
# where 
#  -b  BIN                str    required
#  -p  INITIAL_POINT      str    required
#  -x  TOL_X              float  optional  default 1.0e-9  NOT IMPLEMENTED
#  -f  TOL_F              float  optional  default 1.0e-9  NOT IMPLEMENTED
#  -i  MAX_ITER           int    optional  default 5000    NOT IMPLEMENTED
#  -e  MAX_EVAL           int    optional  default 5000    NOT IMPLEMENTED
#  -d  DISPLAY_PRECISION  int    optional  default 9
#  -v  VERBOSE            int    optional  default 0
#  -a  ADAPTIVE_SCALING   int    optional  default 0
#  -s  SIMPLEX_SCALING    float  optional  default 1.0
#
# Examples:
#  ./run.sh -b nm-ackley -p -2.10,-3.04,4.50
# 
# 
###


bin=""
initial_point=""

tol_x=1e-9
tol_f=1e-9
max_iter=5000
max_eval=5000
display_precision=9
verbose=0
adaptive_scaling=0
simplex_scaling=1.0


while getopts "b:p:x:f:i:e:d:v:a:s:" opt; do
    case $opt in
        b) bin=${OPTARG} ;;
        p) initial_point=${OPTARG} ;;
        x) tol_x=${OPTARG} ;;
        f) tol_f=${OPTARG} ;;
        i) max_iter=${OPTARG} ;;
        e) max_eval=${OPTARG} ;;
        d) display_precision=${OPTARG} ;;
        v) verbose=${OPTARG} ;;
        a) adaptive_scaling=${OPTARG} ;;
        s) simplex_scaling=${OPTARG} ;;
    esac
done

if [ -z "$bin" ]; then
      echo "Error: BIN argument empty"
      exit 1
fi
if [ -z "$initial_point" ]; then
      echo "Error: INITIAL_POINT argument empty"
      exit 1
fi

# workaround, as getops cannot read multiple argument for the same option
# we pass the coordinates as numbers separated by commas and here we
# replace all commas with spaces
IFS=','
set -- $initial_point
IFS=$tmp

# since inital_point is now a string, it will be considered as a 
# single argument; as a workaround we `eval` everything
eval "./${bin} ${display_precision} ${verbose} ${adaptive_scaling} ${simplex_scaling} $@"
