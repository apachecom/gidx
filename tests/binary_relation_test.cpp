//
// Created by inspironXV on 8/17/2018.
//
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include "../binary_relation.h"


static void binary_relation_size(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        binary_relation bin;
        std::vector<binary_relation::point> v(points);
        std::srand(std::time(nullptr));

        for (binary_relation::bin_long i = 1; i <= points; ++i)
        {
            ////binary_relation::ulong row = std::rand()%points+1;
            ////binary_relation::ulong tag = std::rand()%points+1;
            binary_relation::point p = std::make_pair(std::make_pair(i,i),i);
            v[i-1] = p;
        }

        bin.build(v.begin(),v.end(),points,points);
        std::fstream fout("bin_rel_"+std::to_string(points),std::ios::out| std::ios::binary);
        bin.save(fout);
        fout.close();
        bin.print_size();
    }
}

BENCHMARK(binary_relation_size)->Arg(6500000)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
