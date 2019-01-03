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

if [ -z $route ]; then
    echo "No path to clique file"
else    
    ./frequency $route
fi
