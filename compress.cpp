// g++ -std=c++11 compress.cpp -o compress -I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64 -O3 -DNDEBUG

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <sdsl/int_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/util.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/select_support.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_arrays.hpp>

uint32_t compressInt(std::string filename) {
    // sdsl::wt_int<sdsl::rrr_vector<63>> wt_int;
    // sdsl::construct(wt_int, filename.c_str(), 4);
    // store_to_file(wt_int, filename + "-wt_int.sdsl");
    std::cout << filename << " size ";

    sdsl::wm_int<sdsl::rrr_vector<63>> wm_int;
    sdsl::construct(wm_int, filename.c_str(), 4);
    store_to_file(wm_int, filename + "-wm_int.sdsl");

    const uint32_t bitSize = size_in_bytes(wm_int)*8;
    std::cout << bitSize << " bits" << std::endl;
    return bitSize;
}

uint32_t compressByte(std::string filename) {
    // sdsl::wt_int<sdsl::rrr_vector<63>> wt_int;
    // sdsl::construct(wt_int, filename.c_str(), 1);
    // store_to_file(wt_int, filename + "-wt_int.sdsl");

    // sdsl::wt_blcd<sdsl::rrr_vector<63>> wt_blcd;
    // sdsl::construct(wt_blcd, filename.c_str(), 1);
    // store_to_file(wt_blcd, filename + "-wt_blcd.sdsl");

    // sdsl::wt_huff<sdsl::rrr_vector<63>> wt_huff;
    // sdsl::construct(wt_huff, filename.c_str(), 1);
    // store_to_file(wt_huff, filename + "-wt_huff.sdsl");
    std::cout << filename << " size ";

    sdsl::wt_hutu<sdsl::rrr_vector<63>> wt_hutu;
    sdsl::construct(wt_hutu, filename.c_str(), 1);
    store_to_file(wt_hutu, filename + "-wt_hutu.sdsl");

    sdsl::wt_huff<sdsl::rrr_vector<63>> wt_huff;
    sdsl::construct(wt_huff, filename.c_str(), 1);
    store_to_file(wt_huff, filename + "-wt_huff.sdsl");

    sdsl::wt_blcd<sdsl::rrr_vector<63>> wt_blcd;
    sdsl::construct(wt_blcd, filename.c_str(), 1);
    store_to_file(wt_blcd, filename + "-wt_blcd.sdsl");
    

    const uint32_t bitSize = size_in_bytes(wt_hutu)*8;
    std::cout << bitSize << " bits" << std::endl;
    return bitSize;
}

uint32_t compressBitmap(std::string filename) {

    std::ifstream Bfile (filename.c_str());

    std::string sB1;
    std::vector<uint32_t> B1;
    // B1
    if (getline (Bfile, sB1))
    {
        Bfile.close();
    }
    else
    {
        std::cerr << "Unable to open file Bfile" << std::endl;
        return -1;
    }
    std::stringstream ssB1(sB1);

    uint32_t number;
    while (ssB1 >> number)
    {
        B1.push_back(number);
    }

    size_t Blen = B1.size();

    sdsl::bit_vector B = sdsl::bit_vector(Blen, 0);
    for (size_t i=0; i <= Blen; i++) {
        if(B1[i] == 1)
        {
            B[i] = 1;
        }
    }

    std::cout << filename << " size ";

    sdsl::rrr_vector<63> rrrb(B);
    store_to_file(rrrb, filename + "-rrr-64.sdsl");

    const uint32_t bitSize = size_in_bytes(rrrb)*8;
    std::cout << bitSize << " bits" << std::endl;

    sdsl::sd_vector<> sdb(B);
    store_to_file(sdb, filename + "-sdb.sdsl");

    return bitSize;
}


int main(int argc, char const *argv[])
{
    if(2 > argc)
    {
        std::cerr << "Modo de uso: " << argv[0] << " RUTA" << std::endl;
        return -1;
    }

    std::cout << "Compressing..." << std::endl;
    // Path to files
    const std::string path(argv[1]);
    std::cout << path << std::endl;

    std::string sXbin = path+".X.bin";
    std::string sB1 = path+".B1";
    // std::string sB1 = path + ".B";
    std::string sB2bin = path+".B2.bin";
    std::string sYbin = path+".Y.bin";
    std::string sB3 = path+".B3";

    uint32_t totalSizeY = 0, totalSizeB3 = 0;

    totalSizeY += compressInt(sXbin);
    // std::cout << "X Done" << std::endl;

    totalSizeY += compressBitmap(sB1);
    // std::cout << "B1 Done" << std::endl;

    totalSizeY += compressByte(sB2bin);
    // std::cout << "B2 Done" << std::endl;

    totalSizeB3 = totalSizeY;

    totalSizeY += compressInt(sYbin);
    // std::cout << "Y Done" << std::endl;

    totalSizeB3 += compressBitmap(sB3);
    // std::cout << "B3 Done" << std::endl;


    std::cout << "total size [bits]" << std::endl; 
    std::cout << "Y " << totalSizeY << std::endl;
    std::cout << "B3 " << totalSizeB3 << std::endl;

    std::cout << std::endl;


    if(3 == argc)
    {
        std::cout << "BPE Y" << totalSizeY/atof(argv[2]) << std::endl;
        std::cout << "BPE B3" << totalSizeB3/atof(argv[2]) << std::endl;
    }

    return 0;
}
