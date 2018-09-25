//
// Created by alpachec on 25-09-18.
//


#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include "../compressed_grammar.h"


compressed_grammar G;
compressed_grammar::parser_tree Tg;


uint nrules;
std::vector<uint> v_nocc;
uint z_size;

std::vector<uint> v_childs;
std::vector<uint> v_node_pos;




using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;




auto offText = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.offsetText(X);
    }

};

auto isTerminal = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.isTerminal(X);
    }

};

auto n_occ = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.n_occ(X);
    }

};

auto first_occ = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.select_occ(X,1);
    }

};


auto occ = [](benchmark::State &st){

    std::srand(std::time(nullptr));


    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        uint oc = (std::rand() % v_nocc[X-1]) + 1;
        auto p = G.select_occ(X,oc);
    }

};
auto _access_to_Z = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));


    for (auto _ : st)
    {
        compressed_grammar::g_long i = std::rand() % z_size + 1;

        auto p = G[i];
    }

};


auto tg_tree_access = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % z_size + 1;
        auto p = Tg[i];
    }

};

auto tg_child = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        if(v_childs[i] == 0)
            continue;
        auto ch = std::rand() % v_childs[i]+1;
            auto p = Tg.child(v_node_pos[i],ch);
    }

};

auto tg_children = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.children(v_node_pos[i]);
    }

};

auto tg_parent = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.parent(v_node_pos[i]);
    }

};


auto tg_leaf_rank = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.leafrank(v_node_pos[i]);
    }

};

auto tg_leaf_num = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.leafnum(v_node_pos[i]);
    }

};

auto tg_nsibling = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.nsibling(v_node_pos[i]);
    }

};

auto tg_child_rank= [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.childrank(v_node_pos[i]);
    }

};

auto tg_lchild = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.lchild(v_node_pos[i]);
    }

};

auto tg_preorder = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.pre_order(v_node_pos[i]);
    }

};


int main (int argc, char *argv[] ){

   /* if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }*/


    std::string collection = "../tests/collections/repetitive/reals/einstein.de.txt";//(argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }
    std::string data;
    uint code = collections_code[collection];

    //std::string file_to_save = "../files/"+std::to_string(code)+".cg";
    std::string file_to_save = "../files/"+std::to_string(code)+".cg";
    fstream fstream1(file_to_save, std::ios::in|std::ios::binary);




    if(!fstream1.is_open())
    {
        std::fstream f(collection, std::ios::in| std::ios::binary);
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

        auto *p_G = new grammar();
        p_G->buildRepair(data);
        G.build(*p_G);
        fstream fstream2(file_to_save, std::ios::out|std::ios::binary);
        G.save(fstream2);
        delete p_G;
    }
    else
    {
        G.load(fstream1);
    }



    nrules = G.n_rules()-1;

    const auto & Z = G.get_Z();

    z_size = Z.size();

    Tg = G.get_parser_tree();

    v_nocc.resize(nrules);

    for (int j = 0; j < nrules ; ++j) {
        v_nocc[j] = G.n_occ(j+1);
    }

    v_node_pos.resize(z_size);
    v_childs.resize(z_size);


    for (int k = 0; k < z_size; ++k) {
        v_node_pos[k] = Tg[k+1];
        v_childs[k] = Tg.children(v_node_pos[k]);

    }




    benchmark::RegisterBenchmark("G offset text operation",offText);
    benchmark::RegisterBenchmark("G is terminal operation",isTerminal);
    benchmark::RegisterBenchmark("G number of occurences operation",n_occ);
    benchmark::RegisterBenchmark("G first occurence operation",first_occ);
    benchmark::RegisterBenchmark("G random occurence operation",occ);
    benchmark::RegisterBenchmark("G operator [] ",_access_to_Z);
    benchmark::RegisterBenchmark("TG operator [] ",tg_tree_access);
    benchmark::RegisterBenchmark("TG preorder ",tg_preorder);
    benchmark::RegisterBenchmark("TG nsibling ",tg_nsibling);
    benchmark::RegisterBenchmark("TG childs ",tg_child);
    benchmark::RegisterBenchmark("TG children ",tg_children);
    benchmark::RegisterBenchmark("TG childrank ",tg_child_rank);
    benchmark::RegisterBenchmark("TG lchild ",tg_lchild);
    benchmark::RegisterBenchmark("TG parent ",tg_parent);
    benchmark::RegisterBenchmark("TG leaf rank ",tg_leaf_rank);
    benchmark::RegisterBenchmark("TG leaf num ",tg_leaf_num);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}