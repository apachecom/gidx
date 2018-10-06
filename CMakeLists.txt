cmake_minimum_required(VERSION 3.8)
project(improved_grammar_index)

set(CMAKE_CXX_STANDARD 14)



set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${PROJECT_SOURCE_DIR}/cmake)

# Guard against in-source builds and bad build-type strings
#
include(ConfigSafeGuards)



#Testing
include(CTest)
#set(LIBS string_dictionaries)

find_package(Threads REQUIRED)
find_package(GTest)
if (GTEST_FOUND)
    set(LIBS ${LIBS} GTest::Main)
else ()

    if (CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-Wall -ansi -Wno-deprecated -pthread)
    endif ()

    if (MSVC)
        #vc 2012 fix for vararg templates
        set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")
    endif ()

    include(ConfigGTest)
    set(LIBS ${LIBS} gtest gtest_main)
endif ()




#Benchmark
# If you want to self-test benchmark lib too, turn me ON
#
set(BENCHMARK_ENABLE_TESTING OFF)

# Configure google micro benchmark
#

# c++11 is required
#
if ((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    #    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
    #    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif ()

include(ConfigGBench)


set(SOURCE_FILES

        SelfGrammarIndex.cpp SelfGrammarIndex.h
        SelfGrammarIndexBS.cpp SelfGrammarIndexBS.h
        SelfGrammarIndexPT.cpp SelfGrammarIndexPT.h
        SelfGrammarIndexPTS.cpp SelfGrammarIndexPTS.h

        ################REPAIR FILES#########################
        utils/repair/arrayG.cpp utils/repair/arrayG.h
        utils/repair/basics.cpp utils/repair/basics.h
        utils/repair/dictionary.cpp utils/repair/dictionary.h
        utils/repair/records.cpp utils/repair/records.h
        utils/repair/heap.cpp utils/repair/heap.h
        utils/repair/hash.cpp utils/repair/hash.h
        utils/repair/RePair.cpp utils/repair/RePair.h
        utils/build_hyb_lz77.h
        ################REPAIR FILES#########################

        binary_relation.cpp binary_relation.h
        compressed_grammar.cpp compressed_grammar.h

        trees/dfuds_tree.cpp trees/dfuds_tree.h
        trees/bp_tree.cpp trees/bp_tree.h

        trees/patricia_tree/compact_patricia_tree.cpp trees/patricia_tree/compact_patricia_tree.h
        trees/patricia_tree/patricia_tree.cpp trees/patricia_tree/patricia_tree.h
        trees/patricia_tree/sampled_patrica_tree.h trees/patricia_tree/sampled_patrica_tree.cpp

        trees/trie/compact_trie.cpp trees/trie/compact_trie.h
        trees/trie/Trie.cpp trees/trie/Trie.h

        utils/grammar.cpp utils/grammar.h
        tests/collections.cpp
        bench/repetitive_collections.h





        )



add_executable(improved_grammar_index main.cpp ${SOURCE_FILES} )

include(internal_utils)

cxx_executable_with_flags(binary_relation_performance "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/binary_relation_test.cpp ${SOURCE_FILES})
cxx_executable_with_flags(grammar_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/utils/grammar_test.cpp ${SOURCE_FILES})
cxx_executable_with_flags(compressed_grammar_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/compressed_grammar_test.cpp ${SOURCE_FILES})
cxx_executable_with_flags(patricia_tree_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/utils/patricia_tree_test.cpp ${SOURCE_FILES})
cxx_executable_with_flags(compact_trie_test "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/compact_trie_test.cpp ${SOURCE_FILES})
cxx_executable_with_flags(self_index_bin_search "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/self_index_bin_search.cpp ${SOURCE_FILES})
cxx_executable_with_flags(self_index_pt "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/self_index_PT.cpp ${SOURCE_FILES})
cxx_executable_with_flags(wavelet_tree_ap "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/wavelet_tree_ap.cpp ${SOURCE_FILES})
cxx_executable_with_flags(wavelet_tree_range "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/wt_range_search.cpp ${SOURCE_FILES})
cxx_executable_with_flags(building_csa_lcp_rmq "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/building_csa_lcp_rmq.cpp ${SOURCE_FILES})
cxx_executable_with_flags(suffixes_grammar "" "${LIBS};benchmark;sdsl;divsufsort;divsufsort64" tests/suffixes_grammar.cpp ${SOURCE_FILES})


cxx_executable_with_flags(idx_locate_performance "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_locate_performance.cpp ${SOURCE_FILES})
cxx_executable_with_flags(idx_extract_performance "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_extract_performance.cpp ${SOURCE_FILES})
cxx_executable_with_flags(idx_build "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/idx_build.cpp ${SOURCE_FILES})
cxx_executable_with_flags(create_files_patterns "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/create_files_patterns.cpp ${SOURCE_FILES})
cxx_executable_with_flags(grammar_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/grammar_benchmark.cpp ${SOURCE_FILES})
cxx_executable_with_flags(binary_relation_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/binary_relation_benchmark.cpp ${SOURCE_FILES})
cxx_executable_with_flags(selfgrammarnindex_benchmark "" "${LIBS};benchmark;slp;hsi;sdsl;divsufsort;divsufsort64;rmqrmmBP;cds" bench/selfgrammarindex_bench.cpp ${SOURCE_FILES})

target_link_libraries(improved_grammar_index sdsl)
target_link_libraries(improved_grammar_index cds)
target_link_libraries(improved_grammar_index divsufsort)
target_link_libraries(improved_grammar_index divsufsort64)
target_link_libraries(improved_grammar_index rmqrmmBP)
target_link_libraries(improved_grammar_index hsi)

