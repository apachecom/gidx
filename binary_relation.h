//
// Created by inspironXV on 8/16/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H
#define IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H


#include <utility>
#include <vector>
#include <sdsl/wavelet_trees.hpp>
#include <fstream>

class binary_relation {

    public:

        typedef unsigned int bin_long;
        typedef sdsl::wt_int<> wavelet_tree;
        ///typedef sdsl::wm_int<sdsl::rrr_vector<>> wavelet_tree;
        typedef sdsl::rrr_vector<> bin_bit_vector_xb;
        typedef sdsl::rrr_vector<> bin_bit_vector_xa;

        typedef sdsl::int_vector<> compressed_seq;
        typedef std::pair< std::pair< unsigned int,  unsigned int> , unsigned int> point;

    protected:
        wavelet_tree SB;
        compressed_seq SL;

        bin_bit_vector_xb XB;
        bin_bit_vector_xa XA;

        bin_bit_vector_xb::rank_1_type xb_rank1;
        bin_bit_vector_xb::select_1_type xb_sel1;
        bin_bit_vector_xb::select_0_type xb_sel0;
        ///sdsl::rrr_vector<>::select_1_type xa_sel1;
        bin_bit_vector_xa::rank_1_type xa_rank1;

    public:
        binary_relation() = default;
        ~binary_relation() = default;

        binary_relation(const binary_relation& );
        void build(std::vector<point>::iterator , std::vector<point>::iterator, const bin_long &, const bin_long&);

        void range(binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , std::vector< std::pair<size_t,size_t>>& );
        bin_long labels(const size_t& , const size_t &) const;
        bin_long first_label_col(const size_t& ) const;

        void load(std::fstream&);
        void save(std::fstream&);

        bin_long n_columns() const ;

        void print_size();
        void print()
        {
                std::cout<<std::endl;
                auto cols = xa_rank1(XA.size());
                auto rows = xb_rank1(XB.size());
                for (size_t j = 0; j < rows ; ++j) {
                        for (size_t i = 0; i < cols; ++i) {
                                std::cout<<labels(j+1,i+1)<<" ";
                        }
                        std::cout<<std::endl;
                }
                std::cout<<std::endl;

                std::cout<<"XA: ";
                for (auto &&i : XA) {
                        std::cout<< i <<" ";
                }
                std::cout<<"\n";
                std::cout<<"XB: ";
                for (auto &&i : XB) {
                        std::cout<< i <<" ";
                }
                std::cout<<"\n";

                std::cout<<"SB: ";
                for (unsigned long i : SB) {
                        std::cout<<(int) i <<" ";
                }
                std::cout<<"\n";
                std::cout<<"SL: ";
                for (int i = 0; i < SL.size() ; ++i) {
                        std::cout<<SL[i]<<" ";
                }
                std::cout<<"\n";
        }
        unsigned long long size_in_bytes() const ;

        binary_relation& operator=(const binary_relation& );

    protected:
        bin_long map(const bin_long&) const;
        bin_long unmap(const bin_long &) const;
};


#endif //IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H

