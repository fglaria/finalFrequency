// g++ -std=c++11 seqTimes.cpp -o seqTimes -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG

#include <set>
#include <map>
#include <string>
#include "sys/times.h"
#include <chrono>
#include <vector>
#include <array>

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/util.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/select_support.hpp>
#include <sdsl/suffix_arrays.hpp>


void readCompressed(const std::string path, sdsl::wm_int<sdsl::rrr_vector<63>> &x_wm,
    sdsl::rrr_vector<63> &b1_rrr, sdsl::wt_hutu<sdsl::rrr_vector<63>> &b2_wt,
    sdsl::wm_int<sdsl::rrr_vector<63>> &y_wm)
{
    // Path to sequences
    const std::string xPath = path + ".X.bin-wm_int.sdsl";
    const std::string b1Path = path + ".B1-rrr-64.sdsl";
    const std::string b2Path = path + ".B2.bin-wt_hutu.sdsl";
    const std::string yPath = path + ".Y.bin-wm_int.sdsl";

    // Read compressed files
    load_from_file(x_wm, xPath.c_str());
    load_from_file(b1_rrr, b1Path.c_str());
    load_from_file(b2_wt, b2Path.c_str());
    load_from_file(y_wm, yPath.c_str());

    return;
}


void reconstructGraph(sdsl::wm_int<sdsl::rrr_vector<63>> &x_wm,
    sdsl::rrr_vector<63> &b1_rrr, sdsl::wt_hutu<sdsl::rrr_vector<63>> &b2_wt,
    sdsl::wm_int<sdsl::rrr_vector<63>> &y_wm, std::map<uint32_t, std::set<uint32_t>> &graph,
    uint64_t &totalNodes, uint8_t &random)
{
    // SDSL variables for B1
    sdsl::rrr_vector<63>::rank_1_type rrrB1_rank(&rrrB1);
    sdsl::rrr_vector<63>::select_1_type rrrB1_sel(&rrrB1);


    std::vector<uint32_t> xRAM(x_wm.size(), 0);
    for(uint64_t i = 0; i < x_wm.size(); ++i)
    {
        xRAM[i] = x_wm[i];
    }

    std::vector<uint8_t> b2RAM(b2_wt.size(), 0);
    for(uint64_t i = 0; i < b2_wt.size(); ++i)
    {
        b2RAM[i] = b2_wt[i];
    }

    std::vector<uint32_t> yRAM(y_wm.size(), 0);
    for(uint64_t i = 0; i < y_wm.size(); ++i)
    {
        yRAM[i] = y_wm[i];
    }

    // How many partitions for this graph
    const uint32_t howManyPartitions = rrrB1_rank(rrrB1.size());

    uint32_t currentY = 0, nextY = yRAM[0];



    // For every partition, let's find neighbors
    for (uint32_t partition = 0; partition < howManyPartitions; ++partition)
    {
        const uint64_t partitionIndex = b1_select(partitionNumber + 1);
        const uint64_t nextPartitionIndex = b1_select(partitionNumber + 2);

        const uint32_t howManyNodesInPartition = nextPartitionIndex - partitionIndex;

        currentY = nextY;
        nextY = yRAM[partition + 1];

        const uint32_t bytesPerNode = (nextY - current_Y)/howManyNodesInPartition;


        if(0 == bytesPerNode)
        {
            for(uint64_t xCurrentIndex = partitionIndex; xCurrentIndex < nextPartitionIndex; ++xCurrentIndex)
            {
                const uint32_t current_node = xRAM[xCurrentIndex];

                for (uint64_t xAdjacentIndex = xCurrentIndex + 1; xI < nextPartitionIndex; ++xI)
                {
                    const uint32_t adjacent_node = xRAM[xAdjacentIndex];

                    graph[current_node].insert(adjacent_node);
                    graph[adjacent_node].insert(current_node);
                }
            }
        }
        else
        {
            // For each current x, search it's neighbors
            for(uint64_t xCurrentIndex = partitionIndex; xCurrentIndex < nextPartitionIndex; ++xCurrentIndex)
            {
                // Get index of first byte of current node
                const uint64_t currentByteIndex = current_Y + bytesPerNode * (xCurrentIndex - partitionIndex);
                // std::cerr << "cBi " << currentByteIndex << " ";

                // Create a bool vector to check if nodes are already neighbors
                const uint32_t howManyPossibleNeighbors = (nextPartitionIndex - xCurrentIndex) - 1;
                std::vector<bool> neighbors(howManyPossibleNeighbors, 0);

                // Check all bytes of nodes
                uint32_t bytesChecked = 0;
                while(bytesChecked != bytesPerNode)
                {
                    // Get byte of current node to check for neighbors
                    const uint8_t maskByteOfCurrent = b2RAM[currentByteIndex + bytesChecked];

                    // For every possible neighbor, get their idexes
                    for(uint64_t xNeighborIndex = xCurrentIndex + 1; xNeighborIndex < nextPartitionIndex; ++xNeighborIndex)
                    {
                        // std::cerr << "xNeighborIndex ";

                        // Get index in vector of bools, to check if already neighbors
                        const uint32_t xNeighborBoolIndex = (xNeighborIndex - xCurrentIndex) - 1;

                        // If not neighbors yet
                        if(!neighbors[xNeighborBoolIndex])
                        {
                            // std::cerr << xNeighborIndex << " ";

                            // Get index of possible neighbor's byte to check
                            const uint64_t neighborByteIndex = current_Y + bytesPerNode * (xNeighborIndex - partitionIndex) + bytesChecked;
                            // std::cerr << " " << neighborByteIndex

                            // Get byte of possible neighbor to check
                            const uint8_t maskBytePossibleNeighbor = b2RAM[neighborByteIndex];

                            // If not zero, they are neighbors!
                            if(maskByteOfCurrent & maskBytePossibleNeighbor)
                            {
                                // Mark as neighbors in vector of bools
                                neighbors[xNeighborBoolIndex] = 1;

                                const uint32_t adjacent_node = xRAM[xNeighborIndex];

                                graph[current_node].insert(adjacent_node);
                                graph[adjacent_node].insert(current_node);
                            }
                        }
                    }

                    ++bytesChecked;
                }
            }
        }
    }

    return;
}

int maint(int argc, char const *argv[])
{
    if(4 > argc)
    {
        std::cerr << "Modo de uso: " << argv[0] << " RUTA_BASE NODES (0:ORDERNADO/1:ALEATORIO)" << std::endl;
        return -1;
    }

    const std::string path(argv[1]);
    uint64_t totalNodes = atoi(argv[2]);
    uint8_t random = atoi(argv[3]);

    const uint8_t iterations = argv[4] ? atoi(argv[4]) : 1;

    // Variables to read compressed sequences
    sdsl::wm_int<sdsl::rrr_vector<63>> x_wm;
    sdsl::rrr_vector<63> b1_rrr;
    sdsl::wt_hutu<sdsl::rrr_vector<63>> b2_wt;
    sdsl::wm_int<sdsl::rrr_vector<63>> y_wm;

    readCompressed(path, x_wm, b1_rrr, b2_wt, y_wm);

    std::map<uint32_t, std::set<uint32_t>> graph;


    for(uint8_t i = 1; i <= iterations; ++i)
    {
        graph.clear();

        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

        reconstructGraph(x_wm, b1_rrr, b2_wt, y_wm, graph, totalNodes, random);

        std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop_time - start_time).count();

        std::cerr << "Time Reconstruction " << i << ": " << duration << " [ms]" << std::endl;
    }

    std::cout << graph.size() << std::endl;
    uint64_t nodeIndex = 0;
    for(const auto & pair : graph)
    {
        // std::cout << pair.first << ": ";
        while(pair.first != nodeIndex)
        {
            std::cout << std::endl;
            ++nodeIndex;
        }

        for(const auto & node : pair.second)
        {
            std::cout << node << " ";
        }

        ++nodeIndex;
        std::cout << std::endl;
    }


    return 0;
}