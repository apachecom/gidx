//
// Created by alpachec on 22-08-18.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include <sdsl/wavelet_trees.hpp>

static void wavelet_tree_ap_big_size(benchmark::State& state)
{
    size_t points = state.range(0);

    for (auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;



        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }

        std::fstream fw("wt", std::ios::out| std::ios::binary);


        for (size_t i = 0; i <data.size() ; ++i) {
            fw << i;
        }

        fw.close();


        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        data.clear();
        {

            int8_t num_bytes = sizeof(size_t);
            sdsl::wt_blcd<> X;
            sdsl::construct_im(X,collection,num_bytes);

            std::cout<<"wt_ap (build on file)\n";
            std::cout<<"size in mb : "<<sdsl::size_in_mega_bytes(X)<<std::endl;
            std::cout<<"lenght : "<<X.size()<<std::endl;
            std::cout<<"alp size : "<<X.sigma<<std::endl;

        }


    }
}
BENCHMARK(wavelet_tree_ap_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA100})->Unit(benchmark::kMillisecond);
BENCHMARK(wavelet_tree_ap_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA200})->Unit(benchmark::kMillisecond);
BENCHMARK(wavelet_tree_ap_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA})->Unit(benchmark::kMillisecond);
BENCHMARK(wavelet_tree_ap_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();