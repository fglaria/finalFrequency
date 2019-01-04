// g++ -std=c++11 reconstructClique.cpp -o reconstructClique -O3 -DNDEBUG
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

int main(int argc, char* argv[])
{
    if(2 > argc)
    {
        std::cerr << "Modo de uso: " << argv[0] << " RUTA" << std::endl;
        return -1;
    }

    // Path to files
    const std::string basePath(argv[1]);

    std::ifstream fileStream;
    std::string lineFile;

    std::map<uint64_t, std::set<uint64_t>> graph;


    // Open cliques file & create cliques vector
    fileStream.open(basePath.c_str(), std::ios::in);

    if(fileStream.is_open())
    {
        // Read line by line
        while(getline(fileStream, lineFile))
        {
            std::stringstream lineStream(lineFile);
            uint64_t node;

            // Put every node on line on a clique set
            std::set<uint64_t> clique;
            while(lineStream >> node)
            {
                clique.insert(node);
            }

            // Put every node on clique on graph
            std::set<uint64_t>::const_iterator itNode1, itNode2;
            for(itNode1 = clique.begin(); itNode1 != std::prev(clique.end()); ++itNode1)
            {
                for(itNode2 = std::next(clique.begin()); itNode2 != clique.end(); ++itNode2)
                {
                    graph[*itNode1].insert(*itNode2);
                    graph[*itNode2].insert(*itNode1);
                }
            }
        }
    }

    // Output graph
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
