//
// Created by alpachec on 24-09-18.
//

//
// Created by alpachec on 16-09-18.
//



#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexPT.h"
#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>
#include "repetitive_collections.h"
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPTS.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


std::vector<std::pair<uint,uint>> rg;
int code = 0;

HybridSelfIndex* idx_hyb;
ri::r_index<>* idx_r;
cds_static::RePairSLPIndex *idx_slp;
SelfGrammarIndexPT* idx_gimp;






auto g_imp_display = [](benchmark::State &st, const std::string& collection)
{


    double mean =0;

    std::fstream g_f(std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);
    idx_gimp = new SelfGrammarIndexPT();
    idx_gimp->load(g_f);
    g_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {

            auto start = timer::now();
            std::string s;
            s.reserve(i.second-i.first+1);
            idx_gimp->display(i.first,i.second,s);
            auto stop = timer::now();
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }



    st.counters["time"] = mean;
    st.counters["time/c"] = mean/rg.size();

    delete idx_gimp;

};




auto hyb_display = [](benchmark::State &st,const std::string & collection)
{

    double mean;
    std::string filename = std::to_string(collections_code[collection]);
    char* _f = (char *)filename.c_str();
    idx_hyb = new HybridSelfIndex(_f);

    for (auto _ : st)
    {
        mean =0;

        for (auto &&  i : rg )
        {

            auto start = timer::now();
            //idx_hyb->locate((uchar*)(i.c_str()),m,&_Occ,&_occ);
            uint m = i.second-i.first+1;
            unsigned char *s;
            idx_hyb->extract(i.first,m,&s);

            auto stop = timer::now();

            //delete s;

            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

    st.counters["time"] = mean;
    st.counters["time/c"] = mean*1.0/rg.size();
    delete idx_hyb;

};

auto r_display = [](benchmark::State &st)
{
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {

            auto start = timer::now();

            //auto occ = idx_r->locate_all(i);


            auto stop = timer::now();

            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }

    st.counters["time"] = mean;
    st.counters["time/c"] = mean*1.0/rg.size();

    delete idx_r;

};

auto slp_display = [](benchmark::State &st, const std::string & collection)
{

    double mean ;
    std::string filename = std::to_string(collections_code[collection]);
    char* _f = (char *)filename.c_str();

    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    if (q == 0)
    {
        std::cout<<"Error loading slp file\n";
        return;
    }

    for (auto _ : st)
    {
        mean =0;
        for (auto &&  i : rg )
        {

            auto start = timer::now();
                unsigned char * s = idx_slp->RePairSLPIndex::extract(i.first,i.second);
            auto stop = timer::now();
            delete [] s;
            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }

    st.counters["time"] = mean;
    st.counters["time/c"] = mean*1.0/rg.size();
    delete idx_slp;

};


int main (int argc, char *argv[] ){


    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }


    std::string collection = (argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }

    code = collections_code[collection];


    unsigned int num_ranges = atoi(argv[2]);
    //patt.reserve(num_patterns);

    std::string rg_file = argv[3]+ std::to_string(code)+".rg";
    std::fstream f(rg_file, std::ios::in| std::ios::binary);

    if(!f.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        return 0;
    }
    std::string buff;
    while (!f.eof() && std::getline(f, buff)) {
        if(buff.size() > 1){
            uint l,r;
            sscanf(buff.c_str(),"%u %u",&l,&r);
            rg.emplace_back(l,r);
        }
    }

    f.close();



    benchmark::RegisterBenchmark("Grammar-Improved-Index"    , g_imp_display, collection);
    benchmark::RegisterBenchmark("SLP-Index", slp_display,collection);
    //benchmark::RegisterBenchmark("Hyb-Index", hyb_display,collection);

   /* /////////R-Index
    fstream rf(filename+".ri",std::ios::in|std::ios::binary);
    bool fast;

    //fast or small index?
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);
    benchmark::RegisterBenchmark("R-Index", r_display);*/










    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}