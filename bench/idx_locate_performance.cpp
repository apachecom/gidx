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


std::vector<std::string> patt;
int code = 0;
size_t lenght;

HybridSelfIndex* idx_hyb = NULL;
ri::r_index<>* idx_r= NULL;
cds_static::RePairSLPIndex *idx_slp = NULL;
SelfGrammarIndexPT* idx_gimp;
SelfGrammarIndexPTS* idx_gimp_pts;
SelfGrammarIndexBS* idx_gimpbs;


auto g_imp_locate = [](benchmark::State &st)
{

    size_t nocc = 0;
    double mean =0;


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            //  std::cout<<i<<std::endl;
            auto start = timer::now();
            sdsl::bit_vector _occ(lenght);
            idx_gimp->locate(i, _occ);
            sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    delete idx_gimp;

};




auto g_imp_pts_locate = [](benchmark::State &st, const std::string & collection, const uint8_t& sampling)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);

    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

          //  std::cout<<i<<std::endl;
            auto start = timer::now();
            sdsl::bit_vector _occ(lenght,0);
            idx_gimp_pts->locate(i, _occ);
            sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;

    delete idx_gimp_pts;

};

auto g_imp_bs_locate = [](benchmark::State &st)
{

    size_t nocc = 0;
    double mean =0;


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            //std::cout<<i<<std::endl;
            auto start = timer::now();
            sdsl::bit_vector _occ(lenght);
            idx_gimpbs->locate(i, _occ);
            sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();




            nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    delete idx_gimpbs;

};


auto hyb_locate = [](benchmark::State &st)
{
    size_t nocc = 0;
    double mean = 0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {
            unsigned long _Occ;
            unsigned long* _occ;
            unsigned int m = (unsigned int) i.size();
            //std::cout<<i<<std::endl;
            auto start = timer::now();

            idx_hyb->locate((uchar*)(i.c_str()),m,&_Occ,&_occ);

            auto stop = timer::now();

            nocc += _Occ;

            ///delete _occ;

            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    delete idx_hyb;

};

auto r_locate = [](benchmark::State &st)
{
    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            auto start = timer::now();

            auto occ = idx_r->locate_all(i);

            auto stop = timer::now();
            nocc += occ.size();

            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;

    delete idx_r;

};

auto slp_locate = [](benchmark::State &st)
{

    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {
            uint occs;
            uchar* tt = (uchar*)(i.c_str());
           // std::cout<<tt<<std::endl;
            auto start = timer::now();
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt, i.length(), &occs);
            auto stop = timer::now();

            delete pos;

            nocc += occs;

            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;

    delete idx_slp;

};


int main (int argc, char *argv[] ){

    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }

    std::string collection(argv[1]);
    if(collections_code.find(collection) == collections_code.end())
        std::cout<<"bad parameters (not collection code found)....";

    std::fstream f_c(collection, std::ios::in| std::ios::binary);

    std::string buff;
    while (!f_c.eof() && std::getline(f_c, buff)) {
        lenght += buff.size();
    }

    code = collections_code[collection];

    unsigned int num_patterns = atoi(argv[2]);
    //patt.resize(num_patterns);


    std::string pattern_file(argv[3]);
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
        return 0;
    }

    while (!f.eof() && std::getline(f, buff)) {
        if(buff.size() > 1){
            patt.push_back(buff);
        }
    }

    f.close();


    /////GRAMMAR IDEXES

    std::fstream g_f(std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);
    idx_gimp = new SelfGrammarIndexPT();
    idx_gimp->load(g_f);

    benchmark::RegisterBenchmark("Grammar-Improved-Index"    , g_imp_locate);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<8>" , g_imp_pts_locate,collection,8);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<16>", g_imp_pts_locate,collection,16);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<32>", g_imp_pts_locate,collection,32);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<64>", g_imp_pts_locate,collection,64);



    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    idx_gimpbs->load(gbs_f);

    benchmark::RegisterBenchmark("Grammar-Binary-Search-Index", g_imp_bs_locate);



    /////////SLP
    std::string filename = std::to_string(collections_code[collection]);
    char* _f = (char *)filename.c_str();


    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);
    if (q != 0)
    {
        benchmark::RegisterBenchmark("SLP-Index", slp_locate);
    }


    /////////R-Index
    fstream rf(filename+".ri",std::ios::in|std::ios::binary);
    bool fast;

    //fast or small index?
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);
    benchmark::RegisterBenchmark("R-Index", r_locate);
    ///////Hyb Index

    idx_hyb = new HybridSelfIndex(_f);
    benchmark::RegisterBenchmark("Hyb-Index", hyb_locate);




    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}