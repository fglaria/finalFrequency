# finalFrequency
## Compressing graph by clustering cliques by frequency

This is a resume on how to use this beautiful work.


## Preparing for using it...
First, you need to obtain a file with a list of maximal cliques of the graph to be compressed, listing each clique as a line, and each node on that clique as an index.
To do this, we recomend using [Darren Strash's Quick Cliques](https://github.com/darrenstrash/quick-cliques)


## Compressing...
It's separated into two programs, **frequency** and **compress**, each one with it's own shell script and C++ file.

### frequency.sh
This script receives with *-i* option a route to file listing maximal cliques of a graph to be compressed.

As an option, it can be called with *-c* to compile it's corresponding *.cpp* file.
**If it hasn't been called before, you should include this option.**

The output of this script are new created files with same name as input clique file, but with extensions:

- **.X**: Sequence of numbered nodes on partitions.
- **.B1**: Bitmaps pointing with 1 where a partitions begins.
- **.B2**: Sequence of bytes representing bitmaps showing on wich cliques a node could be.
- **.Y**: Sequence of bytes present on each partition.

Each of these have their own binary file.

There are some additional files for testing. Shouldn't be deleted.

### compress.sh
This script receives with *-i* option a route to directory where the output of *frequency* are, using their base common name.

Compressing options for **B1** is -1 (rrr|sdb), and for **B2** is -2 (hutu|huff|blcd), wich define what kind of structure to use for compression (for testing, only *-1 rrr -2 hutu* are developed).

As last option, it can be called with *-c* to compile it's corresponding *.cpp* file.
**If it hasn't been called before, you should include this option.**

The output of this script are new created compressed files, using [Simon Gog's SDSL (Succinct Data Structure Library)](https://github.com/simongog/sdsl-lite).


## Testing

### times.sh
To test how fast the graph can be reconstructed, listing nodes in order or at random, this is script is used.

This script receives with *-i* option a route to directory where the output of *compress* are, using their base common name.

With option *-n* indicates how many nodes the graph contains. Option *-r* specifies if graph is reconstructed in order or at random.

As last option, it can be called with *-c* to compile it's corresponding *.cpp* file.
**If it hasn't been called before, you should include this option.**

