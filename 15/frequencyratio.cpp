// g++ -std=c++11 frequency.cpp -o frequency -O3 -DNDEBUG

#include <iostream>
#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <functional>

#include <algorithm>

#include <algorithm>


template <class type> void writeFileandBinary(std::vector<type> numbers, std::ofstream &seq, std::ofstream &seqbin){
    type *buffer = new type[numbers.size()];
    uint32_t end = numbers.end() - numbers.begin();

    for(uint32_t i=0; i<end; ++i)
    {
        seq << (uint32_t) numbers[i] << " ";
        buffer[i] = numbers[i];
    }

    seqbin.write((char *)&*buffer, (numbers.size())*sizeof(type));
    delete[] buffer;
}




int main(int argc, char* argv[]) {

    if(2 > argc)
    {
        std::cerr << "Modo de uso: " << argv[0] << " RUTA" << std::endl;
        return -1;
    }

    // Path to files
    const std::string basePath(argv[1]);
    //const std::string outPath(argv[2]);

    const std::string cliqueFile = basePath;// + ".cliques";

    std::ifstream fileStream;
    std::string lineFile;

    std::set<uint32_t>::iterator sc;
    std::vector<std::set<uint32_t>> cliques;
    std::map<uint32_t, uint32_t> countNodes;
    std::unordered_map<uint32_t, uint32_t> fNodes;
    std::map<uint32_t, std::set<uint32_t>> nodeCliques;
    std::unordered_map<uint32_t, uint32_t>::iterator msc;
    uint32_t howManyCliques = 0;


    // Open cliques file & create cliques vector
    fileStream.open(cliqueFile.c_str(), std::ios::in);

    if(fileStream.is_open())
    {
        while(getline(fileStream, lineFile))
        {
            std::stringstream lineStream(lineFile);
            uint32_t node;
            std::set<uint32_t> clique;

            while(lineStream >> node)
            {
                clique.insert(node);
                //countNodes[node] += 1;
                fNodes[node] += 1;

                // Check if clique exist to insert node, create it and insert node if not
                std::map<uint32_t, std::set<uint32_t>>::iterator iter = nodeCliques.find(node);
                if(nodeCliques.end() == iter)
                {
                    std::set<uint32_t> us;
                    us.insert(howManyCliques);
                    nodeCliques[node] = us;
                }
                else
                {
                    iter->second.insert(howManyCliques);
                }
            }

	    for(sc=clique.begin(); sc!=clique.end();sc++){
		countNodes[*sc] += clique.size();
	    }
            cliques.push_back(clique);

            ++howManyCliques;
        }
    }
    else
    {
        std::cerr << "No clique file" << std::endl;
        return -1;
    }

    for(msc=fNodes.begin(); msc!=fNodes.end();msc++){
	countNodes[msc->first] = (uint32_t)countNodes[msc->first]/msc->second;
    }
    std::cerr << "First files" << std::endl;

    // std::cerr << "cliques: " << cliques.size() << std::endl;
    // std::cerr << "countNodes: " << countNodes.size() << std::endl;
    // std::cerr << "nodeCliques: " << nodeCliques.size() << std::endl;
    // std::cerr << "howManyCliques: " << howManyCliques << std::endl;

    // for(const auto &cSet: cliques)
    // {
    //     for(const auto &n: cSet)
    //     {
    //         std::cout << n << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // for(const auto &pair : countNodes)
    // {
    //     std::cout << pair.first << " " << pair.second << std::endl;
    // }


    // Order nodes by popularity
    typedef std::function<bool(std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>)> Comparator;

    Comparator compFunctor = [](std::pair<uint32_t, uint32_t> elem1 ,std::pair<uint32_t, uint32_t> elem2)
    {
        if (elem1.second == elem2.second)
        {
            return elem1.first < elem2.first;
        }

        return elem1.second >= elem2.second;
    };

    std::set<std::pair<uint32_t, uint32_t>, Comparator> popNodes(countNodes.begin(), countNodes.end(), compFunctor);
    countNodes.clear();

    std::cerr << "Ordered" << std::endl;


    // std::cerr << "popNodes: " << popNodes.size() << std::endl;

    // for(const auto &pair: popNodes)
    // {
    //     std::cout << pair.first << " " << pair.second;
    //     std::cout << std::endl;
    // }



    // Bool vector to check if a clique is not repeated
    std::vector<bool> uniqueCliques(howManyCliques, 0);
    uint32_t uniqueCount = 0;
    std::vector<std::vector<uint32_t>> popularCliques;

    // For every popular node, from most to less popular
    for(const auto &pair : popNodes)
    {
        // std::cerr << "FOR1" << std::endl;
        const uint32_t &popNode = pair.first;
        // std::cerr << popNode << ": ";

        // Get set of cliques of popular node
        const std::set<uint32_t> &popCliques = nodeCliques[popNode];

        std::vector<uint32_t> uniqueVector;

        uint8_t unique = false;
        for(const auto &cliqueIndex : popCliques)
        {
            if(0 == uniqueCliques[cliqueIndex])
            {
                uniqueCliques[cliqueIndex] = 1;
                // std::cout << cliqueIndex << " ";
                uniqueVector.push_back(cliqueIndex);
                unique = true;
                ++uniqueCount;
            }
        }

        if(unique)
        {
            // std::cout << std::endl;
            popularCliques.push_back(uniqueVector);
        }
    }

    for (uint32_t i = 0; i < uniqueCliques.size(); ++i) {
        if(0 == uniqueCliques[i])
        {
            // std::cout << i << std::endl;
            // std::cerr << i << std::endl;
            ++uniqueCount;
            std::vector<uint32_t> lonely;
            lonely.push_back(i);
            popularCliques.push_back(lonely);
        }
        // else {
        //     std::cerr << "ELSE" << std::endl;
        // }
    }


    nodeCliques.clear();
    uniqueCliques.clear();
    // std::cerr << "uniqueCount: " << uniqueCount << std::endl;
    // std::cerr << "popularCliques: " << popularCliques.size() << std::endl;


    std::cerr << "Uniqued" << std::endl;


    // uint32_t zeros = std::count(uniqueCliques.begin(), uniqueCliques.end(), 0);

    // std::cerr << "uniqueCliques: " << uniqueCliques.size() << std::endl;
    // std::cerr << "uniqueCount: " << uniqueCount << std::endl;
    // std::cerr << "zeros: " << zeros << std::endl;

    // for (const auto & v : popularCliques)
    // {
    //     for (const auto & n : v)
    //     {
    //         std::cerr << n << " ";
    //     }

    //     std::cerr << std::endl;
    // }



    std::vector<std::set<uint32_t>> X;
    std::vector<uint8_t> B1, B3;
    std::vector<std::vector<uint8_t>> B2;
    std::vector<uint32_t> Y;
    Y.push_back(0);


    // popularCliques, cliques
    // std::cerr << "Generating partitions" << std::endl;


    // For every popular list of cliques
    for(const auto &popCliques : popularCliques)
    {
        const uint32_t howManyCliques = popCliques.size();
        // std::cerr << howManyCliques << " ----" << std::endl;

        std::set<uint32_t> partitionNodes;

        for(const auto &cliqueIndex : popCliques)
        {
            // std::cout << partitionNodes.size() << " --" << std::endl;
            for(const auto &node : cliques[cliqueIndex])
            {
                partitionNodes.insert(node);
            }
        }
        // std::cerr << "Done partition " << partitionNodes.size() << std::endl;



        const uint32_t bytesPerX = ((howManyCliques - 1) / 8) + 1;

        // const uint32_t howManyBytes = howManyCliques != 1 ? bytesPerX * partitionNodes.size() : 1;
        const uint32_t howManyBytes = howManyCliques != 1 ? bytesPerX * partitionNodes.size() : 0;
        // const uint32_t howManyBytes = bytesPerX * partitionNodes.size();

        // std::cerr << "howManyCliques: " << howManyCliques << std::endl;
        // std::cerr << "bytesPerX: " << bytesPerX << std::endl;
        // std::cerr << "partitionNodes: " << partitionNodes.size() << std::endl;
        // std::cerr << "howManyBytes: " << howManyBytes << std::endl;

        X.push_back(partitionNodes);
        B1.push_back('1');
        B1.insert(B1.end(), partitionNodes.size() - 1, '0');
        Y.push_back(howManyBytes + Y.back());
	
	B3.push_back('1');

        if(0 != howManyBytes)
        {
            // std::cerr << "B2" << std::endl;
            // std::vector<uint8_t> b2Partition(howManyBytes, 0);

            std::vector<uint8_t> b2Partition(howManyBytes, 0);
	    
	    if(1 < howManyBytes)
	    { 
		B3.insert(B3.end(), howManyBytes - 1, '0');
	    }

            for(uint32_t popCliqueIndex = 0; popCliqueIndex < popCliques.size(); ++popCliqueIndex)
            {
                const std::set<uint32_t> &nClique = cliques[popCliques[popCliqueIndex]];

                for(const auto &node : nClique)
                {
                    std::set<uint32_t>::const_iterator partitionIter = partitionNodes.find(node);
                    if(partitionNodes.cend() != partitionIter)
                    {
                        // Get index of node on partitionNode based on distance between its begining and iterator
                        const uint32_t nodeIndex = std::distance(partitionNodes.cbegin(), partitionIter);

                        const uint32_t b2Index = (bytesPerX * nodeIndex) + (popCliqueIndex / 8);
                        const uint8_t bitIndex = popCliqueIndex % 8;

                        b2Partition[b2Index] |= 1 << bitIndex;
                    }
                }
            }
	    // std::cerr << "B2 for" << std::endl;

            B2.push_back(b2Partition);
        }
	// std::cerr << "B2 done" << std::endl;

        // for(const auto &node : partitionNodes)
        // {
        //     std::cerr << node << " ";
        // }
        // std::cerr << std::endl;

        // for(const auto &byte : b2Partition)
        // {
        //     std::cerr << binary::print(byte, 8, false) << " ";
        // }
        // std::cerr << std::endl;

        // uint32_t i = 0;
        // for (const auto &node : partitionNodes) {
        //     std::cerr << node << ": ";

        //     for (int j = 0; j < bytesPerX; ++j) {
        //         std::cerr << binary::print(b2Partition[i*bytesPerX + j], 8, false) << " ";
        //     }

        //     std::cerr << std::endl;
        //     ++i;
        // }

        // return 0;
    }

    B1.push_back('1');

    // std::cerr << "X size " << X.size() << std::endl;
    // std::cerr << "B1 size " << B1.size() << std::endl;
    // std::cerr << "B2 size " << B2.size() << std::endl;
    // std::cerr << "Y size " << Y.size() << std::endl;

    std::cerr << "Partitioned" << std::endl;



    /* WRITING FILES */
    std::string sXbin = basePath+".X.bin";
    std::string sB1 = basePath+".B1";
    // std::string sB1 = basePath + ".B";
    std::string sB2bin = basePath+".B2.bin";
    std::string sYbin = basePath+".Y.bin";
    std::string sB3 = basePath+".B3";

    // File output
    std::ofstream xFile(basePath + ".X", std::ofstream::out | std::ofstream::trunc);
    std::ofstream b1File(sB1, std::ofstream::out | std::ofstream::trunc);
    std::ofstream b2File(basePath + ".B2", std::ofstream::out | std::ofstream::trunc);
    std::ofstream yFile(basePath + ".Y", std::ofstream::out | std::ofstream::trunc);
    std::ofstream b3File(sB3, std::ofstream::out | std::ofstream::trunc);

    std::ofstream xFileBin(sXbin, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    std::ofstream b1FileBin(basePath + ".B1.bin", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    std::ofstream b2FileBin(sB2bin, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    std::ofstream yFileBin(sYbin, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    //std::ofstream b3FileBin(basePath + ".B3.bin", std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

    std::vector<uint32_t> vX;
    std::vector<uint8_t> vB2;

    for(auto x : X)
    {
        for(auto n : x)
        {
            vX.push_back(n);
        }
    }

    for(auto b2 : B2)
    {
        for(auto b : b2)
        {
            vB2.push_back(b);
        }
    }


    writeFileandBinary(vX, xFile, xFileBin);
    xFile.close();
    // writeFileandBinary(B1, b1File, b1FileBin);
    // b1File.close();
    writeFileandBinary(vB2, b2File, b2FileBin);
    b2File.close();
    writeFileandBinary(Y, yFile, yFileBin);
    yFile.close();

    X.clear();
    B2.clear();
    Y.clear();

    // for(auto x : X)
    // {
    //     for(auto n : x)
    //     {
    //         // std::cout << n << " ";
    //         xFile << n << " ";
    //     }
    //     // std::cout << std::endl;
    // }
    // xFile.close();

    for(auto b1 : B1)
    {
        b1File << b1 << " ";
    }
    b1File.close();
    
    B1.clear();

    
    for(auto b3 : B3)
    {
	b3File << b3 << " ";
    }
    b3File.close();

    B3.clear();

    // for(auto b2 : B2)
    // {
    //     for(auto byte : b2)
    //     {
    //         b2File << unsigned(byte)  << " ";
    //     }
    // }
    // b2File.close();

    // for(auto y : Y)
    // {
    //     yFile << y << " ";
    // }
    // yFile.close();


    // std::cerr << "Written" << std::endl;


    return 0;
}
