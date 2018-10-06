#!/bin/bash


mkdir results
cd cmake-build-release

#./idx_build ../tests/collections/repetitive/reals/cere
#./idx_build ../tests/collections/repetitive/reals/coreutils
#./idx_build ../tests/collections/repetitive/reals/einstein.de.txt
#./idx_build ../tests/collections/repetitive/reals/einstein.en.txt
#./idx_build ../tests/collections/repetitive/reals/Escherichia_Coli
#./idx_build ../tests/collections/repetitive/reals/influenza
#./idx_build ../tests/collections/repetitive/reals/kernel
#./idx_build ../tests/collections/repetitive/reals/para
#./idx_build ../tests/collections/repetitive/reals/world_leaders

#pseudo reals
#./idx_build ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.1
#./idx_build ../tests/collections/repetitive/pseudoreal/dblp.xml.00001.1
#./idx_build ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.2
#./idx_build ../tests/collections/repetitive/pseudoreal/blp.xml.00001.2
#./idx_build ../tests/collections/repetitive/pseudoreal/dna.001.1
#./idx_build ../tests/collections/repetitive/pseudoreal/english.001.2
#./idx_build ../tests/collections/repetitive/pseudoreal/proteins.001.1
#./idx_build ../tests/collections/repetitive/pseudoreal/sources.001.2


./create_files_patterns ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.1 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/dblp.xml.00001.1 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.2 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/dblp.xml.00001.2 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/dna.001.1 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/english.001.2 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/proteins.001.1 10 1000 10
./create_files_patterns ../tests/collections/repetitive/pseudoreal/sources.001.2 10 1000 10

#./create_files_patterns ../tests/collections/repetitive/reals/cere  10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/coreutils 10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/einstein.de.txt   10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/einstein.en.txt   10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/Escherichia_Coli  10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/influenza 10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/kernel    10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/para  10  10000   10
#./create_files_patterns ../tests/collections/repetitive/reals/world_leaders 10  10000   10

#./idx_extract_performance ../tests/collections/repetitive/reals/cere 10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/coreutils    10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/einstein.de.txt  10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/einstein.en.txt  10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/influenza    10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/kernel   10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/para 10000 ../files/
#./idx_extract_performance ../tests/collections/repetitive/reals/world_leaders    10000 ../files/    



#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.de.txt  10000 ../files/ --benchmark_out=../results/einstein_de_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt  10000 ../files/ --benchmark_out=../results/einstein_en_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/kernel   10000 ../files/		--benchmark_out=../results/kernel_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10000 ../files/ --benchmark_out=../results/escherichia_coli_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/influenza    10000 ../files/	--benchmark_out=../results/influenza_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/para 10000 ../files/		--benchmark_out=../results/para_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/coreutils    10000 ../files/ 	--benchmark_out=../results/coreutils_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/cere 10000 ../files/  		--benchmark_out=../results/cere_out.csv --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/world_leaders    10000 ../files/	--benchmark_out=../results/word_leaders_out.csv --benchmark_out_format=csv



./idx_locate_performance ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.1 1000 ../files/		--benchmark_out=../results/dblp.xml.0001.1.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/dblp.xml.00001.1 1000 ../files/		--benchmark_out=../results/dblp.xml.00001.1.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/dblp.xml.0001.2 1000 ../files/		--benchmark_out=../results/dblp.xml.0001.2.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/dblp.xml.00001.2 1000 ../files/		--benchmark_out=../results/dblp.xml.00001.2.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/dna.001.1 1000 ../files/		    --benchmark_out=../results/dna.001.1.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/english.001.2 1000 ../files/		--benchmark_out=../results/english.001.2.csv --benchmark_out_format=csv
./idx_locate_performance ../tests/collections/repetitive/pseudoreal/proteins.001.1 1000 ../files/		--benchmark_out=../results/proteins.001.1.csv --benchmark_out_format=csv
