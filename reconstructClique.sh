#! /bin/bash
# set -x

while getopts "i:c" option; do
    case $option in
        i)
            route=$OPTARG
            ;;
        c)
            g++ -std=c++11 reconstructClique.cpp -o reconstructClique -O3 -DNDEBUG
            ;;
    esac
done

if [ -z "$route" ]; then
    echo "No path to files"
else
    ./reconstructClique $route
fi
