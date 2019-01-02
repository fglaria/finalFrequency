#! /bin/bash
# set -x

while getopts "i:1:2:c" option; do
    case $option in
        i)
            route=$OPTARG
            ;;
        1)
            b1Type=$OPTARG
            ;;
        2)
            b2Type=$OPTARG
            ;;
        c)
            g++ -std=c++11 compress.cpp -o compress -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG
            ;;
    esac
done

if [ -z "$route" ]; then
    echo "No path to files"
elif [ -z "$b1Type" ]; then
    echo "No type to compress B1 (rrr|sdb)"
elif [ -z "$b2Type" ]; then
    echo "No type to compress B2 (hutu|huff|blcd)"
else
    # ./compress $route $b1Type $b2Type > $route.log 2> $route.error
    ./compress $route $b1Type $b2Type
fi
