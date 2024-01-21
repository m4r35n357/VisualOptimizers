#!/bin/sh
 
# Usage
# $ ./run.sh -b BIN -p INITIAL_POINT
#       [-t TOLERANCE] [-i MAX_ITERATIONS]
#       [-d DECIMAL_PLACES] [-n NUMBER_FORMAT]
#       [-s SIMPLEX_SCALING]
#         
# where 
#  -b  BIN                str    required
#  -p  INITIAL_POINT      str    required  comma-separated coordinates
#  -t  TOLERANCE          float  optional
#  -i  MAX_ITERATIONS     int    optional
#  -d  DECIMAL_PLACES     int    optional
#  -n  NUMBER_FORMAT      int    optional  0 = fixed point, 1 = exponential
#  -s  SIMPLEX_SCALING    float  optional
#
# Examples:
#  ./run.sh -b nm-ackley -p -2.10,-3.04,4.50

bin=""
initial_point=""

decimal_places=9
number_format=0
tolerance=1.0e-9
max_iterations=5000
simplex_scaling=1.0

user_defaults='./user-defaults'
[ -f $user_defaults ] && . $user_defaults

while getopts "b:p:n:t:i:d:s:" opt; do
    case $opt in
        b) bin=${OPTARG} ;;
        d) decimal_places=${OPTARG} ;;
        n) number_format=${OPTARG} ;;
        t) tolerance=${OPTARG} ;;
        i) max_iterations=${OPTARG} ;;
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

# convert initial points argument to positional parameters before appending to command
IFS=','
set -- $initial_point

./${bin} ${decimal_places} ${number_format} ${tolerance} ${max_iterations} ${simplex_scaling} $@
