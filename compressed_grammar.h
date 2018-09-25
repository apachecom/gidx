//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_COMPRESSED_GRAMMAR_H
#define IMPROVED_GRAMMAR_INDEX_COMPRESSED_GRAMMAR_H


#include <sdsl/rrr_vector.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <inv_perm_support.hpp>
#include "trees/dfuds_tree.h"
#include "utils/grammar.h"
#include "trees/trie/compact_trie.h"

class compressed_grammar {

    public:

        typedef unsigned int g_long;
        typedef grammar plain_grammar;
        typedef  dfuds::dfuds_tree parser_tree;
        //typedef  sdsl::wt_ap<> wavelet_tree;
        typedef  sdsl::wt_gmr<> wavelet_tree;
        typedef  sdsl::sd_vector<> z_vector;
        typedef  sdsl::sd_vector<> y_vector;
        typedef  sdsl::sd_vector<> l_vector;
        typedef  sdsl::int_vector<> compact_perm;
        typedef  sdsl::inv_perm_support<> inv_compact_perm;
    protected:
        parser_tree m_tree;

        z_vector Z; // Z bitmap with the first position of symbols in preorder sequence
        z_vector::rank_1_type rank1_Z;
        z_vector::rank_0_type rank0_Z;
        z_vector::select_1_type select1_Z;
        z_vector::select_0_type select0_Z;

        wavelet_tree X_p;  // X sequence of preorder grammar tree symbols removing first aparation


        compact_perm F; // is a permutation of Xj such that F[i] = Xj only and only if Xj is the ith diferent symbol in preorder
        inv_compact_perm F_inv; //Inv-Permutation of F

        y_vector Y; // marks the rules Xi -> a
        y_vector::rank_1_type rank_Y;
        y_vector::select_1_type select_Y;
        l_vector L; // marks the init position of each Xi in T
        l_vector::select_1_type select_L;


        /*
         * last occ of every simbol in X_p
         * */
        ///ls_occ_vector l_occ_xp;
        /*
         * Trie store the lefth/right most path of every node in the parser tree that is not a leaf
         */
        compact_trie left_path;
        compact_trie right_path;

        /*
         * alphabeth
         * */


        std::vector<unsigned char> alp;

    public:


        compressed_grammar();

        ~compressed_grammar();

        void build(plain_grammar&);
        template <typename Suff>
        void build(plain_grammar&, Suff & sfx )
        {


        }
        void set_X_p( const wavelet_tree& );

        void set_F(const compact_perm& );

        void set_Z( const z_vector &);

        void set_Y( const y_vector &);

        void set_L( const l_vector &);
        /*
         * Return the number of rules
         * */
        const z_vector& get_Z() const{ return Z;}

        g_long n_rules()const;
        /*
         * return the preorder of the parser tree node associated to
         * the i-th occurrence of variable Xi
         * */
        g_long select_occ(const g_long&, const g_long& ) const;

        bool is_first_occ(const g_long&) const;

        /*
         *  return the variable Xj associated to the i-th preorder node
         *  in the parser tree
         * */
        g_long operator[](const g_long& ) const;
        /*
         *  return the number of times that Xj variable appears in the parser tree
         * */
        g_long n_occ(const g_long& ) const;
        /*
         *  return true if the variable X has a production with the form X->a
         *  where a is a terminal symbol
         * */
        bool isTerminal(const g_long&)const;
        /*
         * return the i-th terminal symbol in lexicographical order
         * */
        unsigned char terminal_simbol(const g_long& i) const;
        /*
         * return the i-th terminal rule in lexicographical order
         * */
        g_long terminal_rule(const g_long& i) const;
        /*
         * Return the position in the text associated to the i-th
         * node in preorder in the grammar tree
         * */
        g_long offsetText(const g_long&) const;
        /*
         * Return e const refernce to the parser tree
         * */
        const parser_tree& get_parser_tree() const;
        /*
         * Return the size of the structure in bytes
         * */
        g_long size_in_bytes()const;

        /*
         * Return the pre-order of the node asociated in the left trie to the
         * preorder of a node in parser tree;
         *
         * */
        g_long pre_left_trie( const g_long& ) const;
        /*
         * Return the pre-order of the node asociated in the right trie to the
         * preorder of a node in parser tree;
         *
         * */
        g_long pre_right_trie( const g_long& ) const;

        /*
         * return a const referernces to left/right mosth path tries
         *
         * */
        const compact_trie& get_left_trie() const;
        const compact_trie& get_right_trie() const;
        /*
         * return the size of the original text
         * */

        g_long get_size_text() const;

        const std::vector<unsigned char> get_alp() const;
        /*
         *
         * */
        bool isLastOcc(const g_long &) const;

        void print_size_in_bytes();

        void save(std::fstream&);

        void load(std::fstream&);

        compressed_grammar& operator=(const compressed_grammar&);


    protected:

        void left_most_path(const plain_grammar&);
        void right_most_path(const plain_grammar&);






};


#endif //IMPROVED_GRAMMAR_INDEX_COMPRESSED_GRAMMAR_H
