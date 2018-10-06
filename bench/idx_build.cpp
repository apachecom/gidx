//
// Created by alpachec on 16-09-18.
//


#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexPT.h"
#include "../SelfGrammarIndexBS.h"
#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>

#include "repetitive_collections.h"
#include "../SelfGrammarIndexPTS.h"

using namespace cds_static;

#define QGRAM_DEF 4
#define MAX_LENGTH_PATTERN 10

std::string path = "../files/";
/*
 *
auto lz77build = [](benchmark::State &st, const string &file_collection){


    unsigned char br=1;
    unsigned char bs=1;
    unsigned char ss=1;
    lz77index::static_selfindex* idx ;
    std::string filename = std::to_string(collections_code[file_collection])+"lz77";
    char* _f = (char *)filename.c_str();
    char* file = (char *) file_collection.c_str();
    for (auto _ : st)
    {
         idx = lz77index::static_selfindex_lz77::build(file,_f, br, bs, ss);
    }
    st.counters["Size"] = idx->size();
    delete idx;

};


auto lzEndbuild = [](benchmark::State &st, const string &file_collection){


    unsigned char br=1;
    unsigned char bs=1;
    unsigned char ss=1;
    lz77index::static_selfindex_lzend* idx ;
    std::string filename = std::to_string(collections_code[file_collection])+"lzEnd";
    char* _f = (char *)filename.c_str();
    char* file = (char *) file_collection.c_str();
    for (auto _ : st)
    {
        idx = lz77index::static_selfindex_lzend::build(file,_f, br, bs, ss);
    }
    st.counters["Size"] = idx->size();
    delete idx;

};

*/
auto hybbuild = [](benchmark::State &st, const string &file_collection){

    std::string filename = path+std::to_string(collections_code[file_collection]);
    char* _f = (char *)filename.c_str();

    char* input_file_parser = (char*)file_collection.c_str();
    char parser_file [] = "lz77_parser_file_hyb.lz";
    build_hyb_lz77(input_file_parser,parser_file);

    HybridSelfIndex* index;

    // saving the index ...

    for (auto _ : st)
    {
        index = new HybridSelfIndex(parser_file,MAX_LENGTH_PATTERN, _f);
    }

    index->saveStructure();
    st.counters["Size"] = index->sizeDS;

    delete index;
};

auto ribuild = [](benchmark::State &st, const string &file_collection){

    std::string filename = path+std::to_string(collections_code[file_collection]);
    char* _f = (char *)filename.c_str();

    fstream f_ridx(path+std::to_string(collections_code[file_collection])+".ri", std::ios::out | std::ios::binary);
    bool fast = false;
    bool sais = false;
    f_ridx.write((char*)&fast,sizeof(fast));

    string input;
    {

        std::ifstream fs(file_collection);
        std::stringstream buffer;
        buffer << fs.rdbuf();
        input = buffer.str();

    }

    ri::r_index<> idx;

    for (auto _ : st)
    {
        idx = ri::r_index<>(input,sais);
    }

    idx.serialize(f_ridx);
    st.counters["Size"] = sdsl::size_in_bytes(idx);

};


auto slpbuild = [](benchmark::State &st, const string &file_collection, int qgram){

    std::string filename = path+std::to_string(collections_code[file_collection]);
    char* _f = (char *)filename.c_str();
    RePairSLPIndex *indexer = new RePairSLPIndex();

    for (auto _ : st)
    {


        ifstream in(file_collection, ifstream::in);

        in.seekg(0,ios_base::end);
        uint tsize = in.tellg();
        in.seekg(0,ios_base::beg);

        if (in.good())
        {
            // Reading the input text
            uchar *text = loadValue<uchar>(in, tsize);
            in.close();

            cout << endl;
            cout << " >> Building the SLP Index for '"<<file_collection<<"':" << endl;
            cout << "    # Input file size: " << tsize << " bytes" << endl;
            cout << "    # " << qgram << "-grams will be additionally indexed in the grammar" << endl;

            // New RePairSLPIndex

            indexer->build(text, tsize, qgram, _f);

        }
        else cerr << endl << "Please, check your input file" << endl << endl;
        //uchar *text = (uchar *) "abracadabra\0";
    }

    indexer->save();
    st.counters["Size"] = indexer->size();
    delete indexer;

};




auto g_imp_build = [](benchmark::State &st, const string &file_collection){

    std::fstream f(file_collection, std::ios::in| std::ios::binary);
    std::string data;
    if(!f.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }
    std::string buff;
    unsigned char buffer[1000];
    while(!f.eof()){
        f.read((char*)buffer,1000);
        data.append((char*) buffer,f.gcount());
    }
/*
    while (!f.eof() && std::getline(f, buff)) {
        data += buff;
    }*/

    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }
    SelfGrammarIndexPT* g_index = new SelfGrammarIndexPT();
    for (auto _ : st)
    {
        g_index->build(data);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();
};

auto g_imp_pts_build = [](benchmark::State &st, const string &file_collection, const uint8_t &sampling){

    std::fstream f(file_collection, std::ios::in| std::ios::binary);
    std::string data;
    if(!f.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }
    std::string buff;
    unsigned char buffer[1000];
    while(!f.eof()){
        f.read((char*)buffer,1000);
        data.append((char*) buffer,f.gcount());
    }


    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }
    SelfGrammarIndexPTS* g_index = new SelfGrammarIndexPTS(sampling);
    for (auto _ : st)
    {
        g_index->build(data);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();
};



auto g_imp_bs_build = [](benchmark::State &st, const string &file_collection){

    std::fstream f(file_collection, std::ios::in| std::ios::binary);
    std::string data;
    if(!f.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }
    std::string buff;
    unsigned char buffer[1000];
    while(!f.eof()){
        f.read((char*)buffer,1000);
        data.append((char*) buffer,f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }
    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    for (auto _ : st)
    {
        g_index->build(data);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gbsidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();
};


int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }
    ///slp_build2();
    //// build improve grammar index patrcia tree /////


    std::string collection(argv[1]);

    // New RePairSLPIndex
    //cout << " >> Building the SLP Index:" << endl;
    benchmark::RegisterBenchmark("R-Index", ribuild, collection);
    benchmark::RegisterBenchmark("Hyb-Index", hybbuild, collection);
    benchmark::RegisterBenchmark("SLP-Index-8", slpbuild,collection,8);
    benchmark::RegisterBenchmark("SLP-Index-16", slpbuild,collection,16);
    benchmark::RegisterBenchmark("SLP-Index-32", slpbuild,collection,32);
    benchmark::RegisterBenchmark("SLP-Index-64", slpbuild,collection,64);
    benchmark::RegisterBenchmark("Grammar-Improved-Index Binary Search", g_imp_bs_build, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index", g_imp_build, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<8>",  g_imp_pts_build, collection, 8);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<16>", g_imp_pts_build, collection, 16);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<32>", g_imp_pts_build, collection, 32);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<64>", g_imp_pts_build, collection, 64);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<128>",  g_imp_pts_build, collection,128);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<256>", g_imp_pts_build, collection, 256);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<512>", g_imp_pts_build, collection, 512);

    /*cout << " >> Building the LZ77-Index:" << endl;
    benchmark::RegisterBenchmark("LZ77-Index", lz77build, collection);
    cout << " >> Building the LZEnd-Index:" << endl;
    benchmark::RegisterBenchmark("LZEnd-Index", lzEndbuild, collection);*/
    //cout << " >> Building R-Index:" << endl;



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}
