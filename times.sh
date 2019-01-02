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
            g++ -std=c++11 times.cpp -o times -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG
            ;;
    esac
done

if [ -z "$route" ]; then
    echo "No path to files"
elif [ -z "$nodes" ]; then
    echo "No number of nodes"
elif [ -z "$random" ]; then
    echo "No random specified"
# elif [ -z "$b1Type" ]; then
#     echo "No type specified for B1 (rrr|sdb)"
# elif [ -z "$b2Type" ]; then
#     echo "No type specified for B2 (hutu|huff|blcd)"
else
    # ./times $route $nodes $random > $route.log 2> $route.error
    ./times $route $nodes $random 
fi
