// g++ -std=c++11 frequency.cpp -o frequency -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG

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


void getNodeNeighbors(sdsl::wm_int<sdsl::rrr_vector<63>> &x_wm,
    sdsl::rrr_vector<63>::rank_1_type &b1_rank, sdsl::rrr_vector<63>::select_1_type &b1_select,
    std::vector<uint8_t> &b2RAM, std::vector<uint32_t> &yRAM,
    std::map<uint32_t, std::set<uint32_t>> &graph, uint64_t current_node)
{
    const uint32_t howManyX = x_wm.rank(x_wm.size(), current_node);

    for (uint32_t xCount = 1; xCount <= howManyX; ++xCount)
    {
        // std::cerr << "N" << current_node << " ";

        const uint64_t xIndex = x_wm.select(xCount, current_node);
        // std::cerr << "nI" << xIndex << " ";

        uint64_t partitionNumber = b1_rank(xIndex + 1) - 1;
        // std::cerr << "pN " << partitionNumber << " ";

        const uint64_t partitionIndex = b1_select(partitionNumber + 1);
        // std::cerr <<  "pI " << partitionIndex << " ";

        const uint64_t nextPartitionIndex = b1_select(partitionNumber + 2);
        // std::cerr <<  "nPI " << nextPartitionIndex << " ";

        const uint32_t howManyNodesInPartition = nextPartitionIndex - partitionIndex;
        // std::cerr <<  "hMB " << howManyNodesInPartition << " ";

        const uint32_t current_Y = yRAM[partitionNumber];
        // std::cerr <<  "cY " << current_Y << " ";

        const uint32_t bytesPerNode = (yRAM[partitionNumber + 1] - current_Y)/howManyNodesInPartition;
        // std::cerr << "bpn " << bytesPerNode << " ";

        // If no bytes per node, all nodes are adjacent
        if(0 == bytesPerNode)
        {
            for (uint64_t xI = partitionIndex; xI < nextPartitionIndex; ++xI)
            {
                if(xIndex != xI)
                {
                    const uint64_t adjacentNode = x_wm[xI];

                    graph[current_node].insert(adjacentNode);
                    graph[adjacentNode].insert(current_node);
                }
            }
        }
        else
        {
            const uint64_t currentByteIndex = current_Y + bytesPerNode * (xIndex - partitionIndex);
            // std::cerr << "cBi " << currentByteIndex << " ";

            std::vector<bool> neighbors(nextPartitionIndex - partitionIndex, 0);
            neighbors[xIndex - partitionIndex] = 1;

            // std::cerr << "xI ";
            uint32_t bytesChecked = 0;
            while(bytesChecked != bytesPerNode)
            {
                const uint8_t maskByteOfCurrent = b2RAM[currentByteIndex + bytesChecked];

                for(uint32_t xI = partitionIndex; xI < nextPartitionIndex; ++xI)
                {
                    if(!neighbors[xI - partitionIndex])
                    {
                        // std::cerr << xI << " ";

                        const uint32_t b2xIbyteIndex = current_Y + bytesPerNode * (xI - partitionIndex);
			// std::cerr << " " << b2xIbyteIndex

                        const uint8_t maskBytePossibleNeighbor = b2RAM[b2xIbyteIndex + bytesChecked];

                        if(maskByteOfCurrent & maskBytePossibleNeighbor)
                        {
                            neighbors[xI - partitionIndex] = 1;

                            const uint64_t xNeighbor = x_wm[xI];

                            graph[current_node].insert(xNeighbor);
                            graph[xNeighbor].insert(current_node);
                        }
                    }
                }

                ++bytesChecked;
            }

            // for (int xI = partitionIndex; xI < nextPartitionIndex; ++xI)
            // {
            //     // If not the same node
            //     if(xIndex != xI)
            //     {
            //         std::cerr << xI << " ";

            //         uint32_t bytesChecked = 0;
            //         while (bytesChecked != bytesPerNode)
            //         {
            //             uint8_t maskByteOfCurrent = b2RAM[currentByteIndex + bytesChecked];

            //             ++bytesChecked;
            //         }
            //     }
            // }
        }

        // std::cerr << std::endl;
    }
    // std::cerr << std::endl;

    return ;
}


void reconstructGraph(sdsl::wm_int<sdsl::rrr_vector<63>> &x_wm,
    sdsl::rrr_vector<63> &b1_rrr, sdsl::wt_hutu<sdsl::rrr_vector<63>> &b2_wt,
    sdsl::wm_int<sdsl::rrr_vector<63>> &y_wm, std::map<uint32_t, std::set<uint32_t>> &graph,
    uint64_t &totalNodes, uint8_t &random)
{
    sdsl::rrr_vector<63>::rank_1_type b1_rank(&b1_rrr);
    sdsl::rrr_vector<63>::select_1_type b1_select(&b1_rrr);

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
    std::cerr << std::endl;

    if(random)
    {
        std::vector<bool> nodesDone(totalNodes, 0);
        uint64_t doneNodesCount = 0;

        srand(time(NULL));

        while(doneNodesCount < totalNodes)
        {
            uint64_t random_node = rand() % totalNodes;
            if(nodesDone[random_node])
            {
                continue;
            }

            nodesDone[random_node] = 1;

            getNodeNeighbors(x_wm, b1_rank, b1_select, b2RAM, yRAM, graph, random_node);

            ++doneNodesCount;
        }
    }
    else
    {
        for (uint64_t ordered_node = 0; ordered_node < totalNodes; ++ordered_node)
        {
            getNodeNeighbors(x_wm, b1_rank, b1_select, b2RAM, yRAM, graph, ordered_node);
        }
    }

    return;
}


int main(int argc, char const *argv[])
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

    // std::map<uint32_t, std::set<uint32_t>> xOnRAM;

    // Read compressed sequences
    readCompressed(path, x_wm, b1_rrr, b2_wt, y_wm);

    std::map<uint32_t, std::set<uint32_t>> graph;

    for(uint8_t i = 1; i <= iterations; ++i)
    {
        graph.clear();

        std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

        reconstructGraph(x_wm, b1_rrr, b2_wt, y_wm, graph, totalNodes, random);

        std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (stop_time - start_time).count();

        std::cerr << "Time Reconstruction " << i << ": " << duration << std::endl;
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
