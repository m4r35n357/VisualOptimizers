#!/bin/sh
 
# Usage
# $ ./run.sh -b BIN -p INITIAL_POINT
#       [-t TOLERANCE] [-i MAX_ITERATIONS]
#       [-d DISPLAY_PRECISION] [-n NUMBER_FORMAT] [-v VERBOSE] 
#       [-a ADAPTIVE_SCALING] [-s SIMPLEX_SCALING]
#         
# where 
#  -b  BIN                str    required
#  -p  INITIAL_POINT      str    required  comma-separated coordinates
#  -t  TOLERANCE          float  optional
#  -i  MAX_ITERATIONS     int    optional
#  -d  DISPLAY_PRECISION  int    optional
#  -n  NUMBER_FORMAT      int    optional  0 = fixed point, 1 = exponential
#  -v  VERBOSE            int    optional  0 = off, 1 = on
#  -a  ADAPTIVE_SCALING   int    optional  0 = off, 1 = on
#  -s  SIMPLEX_SCALING    float  optional
#
# Examples:
#  ./run.sh -b nm-ackley -p -2.10,-3.04,4.50

bin=""
initial_point=""

display_precision=9
number_format=0
verbose=0
tolerance=1.0e-9
max_iter=5000
adaptive_scaling=0
simplex_scaling=1.0

user_defaults='./user-defaults'
[ -f $user_defaults ] && . $user_defaults

while getopts "b:p:n:t:i:d:v:a:s:" opt; do
    case $opt in
        b) bin=${OPTARG} ;;
        d) display_precision=${OPTARG} ;;
        n) number_format=${OPTARG} ;;
        v) verbose=${OPTARG} ;;
        t) tolerance=${OPTARG} ;;
        i) max_iter=${OPTARG} ;;
        a) adaptive_scaling=${OPTARG} ;;
        s) simplex_scaling=${OPTARG} ;;
        p) initial_point=${OPTARG} ;;
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

./${bin} ${display_precision} ${number_format} ${verbose} ${tolerance} ${max_iter} ${adaptive_scaling} ${simplex_scaling} $@
