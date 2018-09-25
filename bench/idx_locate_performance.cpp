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
std::string data_bs;
HybridSelfIndex* idx_hyb;
ri::r_index<>* idx_r;
cds_static::RePairSLPIndex *idx_slp;
SelfGrammarIndexPT* idx_gimp;
SelfGrammarIndexPTS* idx_gimp_pts;
SelfGrammarIndexBS* idx_gimpbs;



std::fstream b_f(       "br",std::ios::out|std::ios::binary);
std::fstream slp_f(     "slpr",std::ios::out|std::ios::binary);
std::fstream r_f(       "rr",std::ios::out|std::ios::binary);
std::fstream hyb_f(       "hybr",std::ios::out|std::ios::binary);
std::fstream g_fpt(     "grpt",std::ios::out|std::ios::binary);
std::fstream g_fpt8(    "grpt8",std::ios::out|std::ios::binary);
std::fstream g_fpt16(   "grpt16",std::ios::out|std::ios::binary);
std::fstream g_fpt32(   "grpt32",std::ios::out|std::ios::binary);
std::fstream g_fpt64(   "grpt64",std::ios::out|std::ios::binary);
std::fstream g_fbs(     "grbs",std::ios::out|std::ios::binary);




auto g_imp_locate = [](benchmark::State &st, const std::string& collection)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);
    idx_gimp = new SelfGrammarIndexPT();
    idx_gimp->load(g_f);
    g_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            auto start = timer::now();

            sdsl::bit_vector _occ(data_bs.size(),0);

            idx_gimp->locate2(i, _occ);

            sdsl::bit_vector::rank_1_type rrr(&_occ);


            auto stop = timer::now();

            nocc += rrr.rank(_occ.size());
            g_fpt << i << " " << nocc << "\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }
    g_fpt.close();
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
    g_f.close();

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            /////std::cout<<i<<std::endl;
            auto start = timer::now();
            sdsl::bit_vector _occ(data_bs.size(),0);
            idx_gimp_pts->locate2(i, _occ);
            sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += rrr.rank(_occ.size());

  /*          switch (sampling){

                case 8:{
                    g_fpt8 << i << " " << nocc << "\n";
                }break;

                case 16:{
                    g_fpt16 << i << " " << nocc << "\n";
                }break;

                case 32:{
                    g_fpt32 << i << " " << nocc << "\n";
                }break;

                case 64:{
                    g_fpt64 << i << " " << nocc << "\n";
                }break;

                default  : break;

            }*/

            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }

/*
    switch (sampling){

        case 8:{
            g_fpt8.close();
        }break;

        case 16:{
            g_fpt16.close();
        }break;

        case 32:{
            g_fpt32.close();
        }break;

        case 64:{
            g_fpt64.close();
        }break;

        default  : break;

    }*/

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;

    delete idx_gimp_pts;

};

auto g_imp_bs_locate = [](benchmark::State &st, const std::string & collection)
{

    size_t nocc = 0;
    double mean =0;

    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    idx_gimpbs->load(gbs_f);
    gbs_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            //std::cout<<i<<std::endl;
            auto start = timer::now();
            sdsl::bit_vector _occ(data_bs.size(),0);
            idx_gimpbs->locate2(i, _occ);
            sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += rrr.rank(_occ.size());
            //g_fbs << i << " " << nocc <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }
    //g_fbs.close();
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
            //hyb_f << i << " " << _Occ <<"\n";
            ///delete _occ;
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }
//    hyb_f.close();
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
//            r_f << i << " " << occ.size() <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }
//    r_f.close();
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
//            slp_f << i << " " << occs <<"\n";
            delete pos;
            nocc += occs;
            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
//    slp_f.close();

    delete idx_slp;

};



auto bt_locate = [](benchmark::State &st)
{

    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {
            uint occs = 0;
            auto start = timer::now();

                size_t pos = data_bs.find(i, 0);
                while(pos != string::npos)
                {
                    ++nocc;

                    occs++;
                    pos = data_bs.find(i,pos+1);
                }

            auto stop = timer::now();
//            b_f << i << " " << occs <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());


        }

    }


//    b_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;



};

int main (int argc, char *argv[] ){

/*
    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }
*/

    std::string collection = (argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }


    std::fstream f_c(collection, std::ios::in| std::ios::binary);

/*  std::string buff;
    while (!f_c.eof() && std::getline(f_c, buff)) {
        lenght += buff.size();
        data_bs+=buff;
    }
*/


    unsigned char buffer[1000];
    while(!f_c.eof()){
        f_c.read((char*)buffer,1000);
        data_bs.append((char*) buffer,f_c.gcount());
    }

    for (int i = 0; i < data_bs.size(); ++i) {
        if(data_bs[i] == 0 || data_bs[i] == 1)
            data_bs[i] = 2;
    }

    f_c.close();
    code = collections_code[collection];

    unsigned int num_patterns = atoi(argv[2]);
    //patt.reserve(num_patterns);

    std::string pattern_file = (argv[3]) + std::to_string(code)+".ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
        return 0;
    }
    std::string buff;
    while (!f.eof() && std::getline(f, buff)) {
        if(buff.size() > 1){
            patt.push_back(buff);
        }
    }

    f.close();

    /////////SLP

   /* std::string filename = string(argv[3]) + std::to_string(collections_code[collection]);
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


*/



    /////GRAMMAR IDEXES


   // benchmark::RegisterBenchmark("Grammar-Binary-Search-Index", g_imp_bs_locate, collection);
   // benchmark::RegisterBenchmark("Grammar-Improved-Index"    , g_imp_locate, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<8>" , g_imp_pts_locate,collection,8);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<16>", g_imp_pts_locate,collection,16);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<32>", g_imp_pts_locate,collection,32);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<64>", g_imp_pts_locate,collection,64);
    //benchmark::RegisterBenchmark("Brute Force-Index"    , bt_locate);


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}