#! /bin/bash
# set -x

while getopts "i:n:r:1:2:c" option; do
    case $option in
        i)
            route=$OPTARG
            ;;
        n)
            nodes=$OPTARG
            ;;
        r)
            random=$OPTARG
            ;;
        c) 
            g++ -std=c++11 seqTimes.cpp -o seqTimes -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG
            ;;
    esac
done

if [ -z "$route" ]; then
    echo "No path to files"
elif [ -z "$nodes" ]; then
    echo "No number of nodes"
elif [ -z "$random" ]; then
    echo "No random specified"
else
    ./seqTimes $route $nodes $random 
fi
