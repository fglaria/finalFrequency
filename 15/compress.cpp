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

    //sdsl::wm_int<sdsl::rrr_vector<63>> wm_int;
    sdsl::wm_int<sdsl::rrr_vector<15>> wm_int;
    sdsl::construct(wm_int, filename.c_str(), 4);
    store_to_file(wm_int, filename + "-wm_int.sdsl");

    const uint32_t bitSize = size_in_bytes(wm_int)*8;
    std::cout << bitSize << " bits" << std::endl;

    return bitSize;
}

uint32_t compressByte(std::string filename, std::string type) {
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

    uint32_t bitSize = 0;

    if("hutu" == type)
    {
        sdsl::wt_hutu<sdsl::rrr_vector<15>> wt_hutu;
        //sdsl::wt_hutu<sdsl::bit_vector_il<>> wt_hutu;
        sdsl::construct(wt_hutu, filename.c_str(), 1);
        store_to_file(wt_hutu, filename + "-wt_hutu.sdsl");

        bitSize = size_in_bytes(wt_hutu)*8;
        std::cout << bitSize << " bits" << std::endl;
    }
    else if("huff" == type)
    {
        sdsl::wt_huff<sdsl::rrr_vector<15>> wt_huff;
        sdsl::construct(wt_huff, filename.c_str(), 1);
        store_to_file(wt_huff, filename + "-wt_huff.sdsl");

        bitSize = size_in_bytes(wt_huff)*8;
        std::cout << bitSize << " bits" << std::endl;
    }
    else if("blcd" == type)
    {
        sdsl::wt_blcd<sdsl::rrr_vector<15>> wt_blcd;
        sdsl::construct(wt_blcd, filename.c_str(), 1);
        store_to_file(wt_blcd, filename + "-wt_blcd.sdsl");

        bitSize = size_in_bytes(wt_blcd)*8;
        std::cout << bitSize << " bits" << std::endl;
    }
    else
    {
        std::cerr << "Byte unknown type: " << type << std::endl;
    }

    return bitSize;
}

uint32_t compressBitmap(std::string filename, std::string type) {

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

    uint32_t bitSize = 0;

    if("rrr" == type)
    {
        sdsl::rrr_vector<63> rrrb(B);
        store_to_file(rrrb, filename + "-rrr-64.sdsl");

        bitSize = size_in_bytes(rrrb)*8;
        std::cout << bitSize << " bits" << std::endl;
    }
    else if("sdb" == type)
    {
        sdsl::sd_vector<> sdb(B);
        store_to_file(sdb, filename + "-sdb.sdsl");

        bitSize = size_in_bytes(sdb)*8;
        std::cout << bitSize << " bits" << std::endl;
    }
    else
    {
        std::cerr << "Bitmap unknown type: " << type << std::endl;
    }

    return bitSize;
}


int main(int argc, char const *argv[])
{
    if(4 > argc)
    {
        std::cerr << "Modo de uso: " << argv[0] << " RUTA B1_type(rrr|sdb) B2_type(hutu|huff|blcd)" << std::endl;
        return -1;
    }

    std::cout << "Compressing..." << std::endl;
    // Path to files
    const std::string path(argv[1]);
    std::cout << path << std::endl;

    const std::string b1Type(argv[2]);
    const std::string b2Type(argv[3]);

    if("rrr" != b1Type && "sbd" != b1Type)
    {
        std::cerr << "Tipo de compresión a B1 inválido (rrr|sdb): " << b1Type << std::endl;
        return -1;
    }

    if("hutu" != b2Type && "huff" != b2Type && "blcd" != b2Type)
    {
        std::cerr << "Tipo de compresión a B2 inválido (hutu|huff|blcd): " << b2Type << std::endl;
        return -1;
    }

    std::string sXbin = path+".X.bin";
    std::string sB1 = path+".B1";
    // std::string sB1 = path + ".B";
    std::string sB2bin = path+".B2.bin";
    std::string sYbin = path+".Y.bin";
    //std::string sB3 = path+".B3";

    uint32_t totalSizeY = 0;//, totalSizeB3 = 0;

    totalSizeY += compressInt(sXbin);
    // std::cout << "X Done" << std::endl;

    totalSizeY += compressBitmap(sB1, b1Type);
    // std::cout << "B1 Done" << std::endl;

    totalSizeY += compressByte(sB2bin, b2Type);
    // std::cout << "B2 Done" << std::endl;

    //totalSizeB3 = totalSizeY;

    totalSizeY += compressInt(sYbin);
    // std::cout << "Y Done" << std::endl;

    //totalSizeB3 += compressBitmap(sB3);
    // std::cout << "B3 Done" << std::endl;


    std::cout << "total size [bits]" << std::endl;
    std::cout << "Y " << totalSizeY << std::endl;
    //std::cout << "B3 " << totalSizeB3 << std::endl;

    std::cout << std::endl;


    /*
    if(3 == argc)
    {
        std::cout << "BPE Y" << totalSizeY/atof(argv[2]) << std::endl;
        std::cout << "BPE B3" << totalSizeB3/atof(argv[2]) << std::endl;
    }
    */

    return 0;
}
