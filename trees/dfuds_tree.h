//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_DFUDS_TREE_H
#define IMPROVED_GRAMMAR_INDEX_DFUDS_TREE_H


#include <sdsl/int_vector.hpp>
#include <sdsl/bp_support_sada.hpp>
#include <sdsl/rrr_vector.hpp>

namespace dfuds {


    class dfuds_tree {

    public:
        typedef sdsl::bit_vector bv;
        typedef sdsl::bp_support_sada<> parenthesis_seq;
        typedef unsigned int dfuds_long;



    protected:
        bv bit_vector;
        parenthesis_seq bps;
        sdsl::rank_support_v<00, 2> rank_00;
        sdsl::select_support_mcl<00, 2> select_00;
        bv::select_0_type select_0;


    public:
        dfuds_tree() = default;

        ~dfuds_tree() = default;

        void build(const sdsl::bit_vector &);

        short root()const;
        /*
         * return the number of nodes in the subtree of the node
         * */
        dfuds_long subtree(const dfuds_long &)const;

        bool isleaf(const dfuds_long &)const;

        /*
         * (preoreder) return the preorder of a node
         * */
        dfuds_long pre_order(const dfuds_long &)const;

        /*
         * (preoreder select) return the i-th node in preorder
         * */
        dfuds_long operator[](const dfuds_long &)const;

        dfuds_long nsibling(const dfuds_long &)const;

        dfuds_long lchild(const dfuds_long &)const;


        /*
         *  return the i-th child of a node
         * */
        dfuds_long child(const dfuds_long &, const dfuds_long &)const;

        /*
         * return the rank of a node between its brothers
         * */
        dfuds_long childrank(const dfuds_long &)const;

        /*
         * return the number of children of a node
         * */
        dfuds_long children(const dfuds_long &v)const;

        /*
         * return the number of leaves in the subtree of a node
         * */
        dfuds_long leafnum(const dfuds_long &)const;

        /*
         * return the rank between the leaf of the left most leaf of a node
         * */
        dfuds_long leafrank(const dfuds_long &) const;

        /*
         * return the i-th leaf
         * */
        dfuds_long leafselect(const dfuds_long &)const;

        /*
         * return the parent of a node
         * */
        dfuds_long parent(const dfuds_long &)const;

        /*
         * dfs preorder over the tree
         * */
        template<typename K>
        void dfs_preorder(const dfuds_long &node, const K &f) const{

            auto keep = f(node, this);
            if (!keep)
                return;
            dfuds_long n = children(node);
            for (dfuds_long i = 0; i < n; ++i) {
                dfs_preorder(child(node, i + 1), f);
            }
        }

        /*
         * dfs posorder over the tree
         *
         * */
        template<typename K>
        void dfs_posorder(const dfuds_long &node, const K &f)const {

            dfuds_long n = children(node);
            for (dfuds_long i = 0; i < n; ++i) {
                dfs_posorder(child(node, i + 1), f);
            }

            f(node);
        }

        /*
         * this function walks a path from root to a leaf
         *
         * */
        template<typename K>
        void path(dfuds_long &node, const K &f) const{
            dfuds_long next_node = 0;

            if (f(node, next_node))
                path(next_node, f);
        }

        template<typename K>
        dfuds_long
        find_child(const dfuds_long &node, dfuds_long &ls, dfuds_long &hs, const K &f) const{

            if (ls+1 >= hs )
            {
                if (f(child(node, hs)) == true)
                    return ls;
                return hs;
            }

            dfuds_long rank_ch = (ls + hs) / 2;
            /*
             * If value < value(node) f must return true
             * */
            f(child(node, rank_ch)) ? (hs = rank_ch-1) : (ls = rank_ch);

            return find_child(node, ls, hs, f);
        }


        dfuds_long size_in_bytes() const;
        double size_in_mega_bytes() const;

        void print_size_in_bytes(const std::string &) const;


        void save(std::fstream &f) const;

        void load(std::fstream &f);

        dfuds_long rank_1(const dfuds_long &) const;


        void print()const;

        dfuds_tree& operator=(const dfuds_tree &);

    protected:
        dfuds_long pred0(const dfuds_long &i) const;

        dfuds_long succ0(const dfuds_long &i) const;

    };

}

#endif //IMPROVED_GRAMMAR_INDEX_DFUDS_TREE_H
