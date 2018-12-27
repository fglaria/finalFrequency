#!/bin/bash

echo "g++ -std=c++11 popular.cpp -o popular -O3 -DNDEBUG"
g++ -std=c++11 popular.cpp -o popular -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG

echo "g++ -std=c++11 compress.cpp -o compress -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG"
g++ -std=c++11 compress.cpp -o compress -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG


echo "./popular dblp-2011/dblp-2011"
./popular dblp-2011/dblp-2011

echo "./compress dblp-2011/dblp-2011 6707236"
./compress dblp-2011/dblp-2011 6707236
