#! /bin/bash
# set -x

while getopts "i:c" option; do
    case $option in
        i)
            route=$OPTARG
            ;;
        c) 
            g++ -std=c++11 frequency.cpp -o frequency -O3 -DNDEBUG
            ;;
    esac
done

if [[ $route ]]; then
    ./frequency $route > $route.log 2> $route.error
else
    echo "No path to cliques file"
fi
