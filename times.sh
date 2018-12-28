#! /bin/bash
# set -x

while getopts "i:c" option; do
    case $option in
        i)
            route=$OPTARG
            ;;
        c) 
            g++ -std=c++11 times.cpp -o times -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG
            ;;
    esac
done

if [[ $route ]]; then
    ./times $route > $route.log 2> $route.error
else
    echo "No path to files"
fi