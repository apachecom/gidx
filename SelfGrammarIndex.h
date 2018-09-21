//
// Created by inspironXV on 8/16/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H


#include <string>
#include <sdsl/bit_vectors.hpp>
#include "compressed_grammar.h"
#include "binary_relation.h"

class SelfGrammarIndex {

    public:
        typedef compressed_grammar grammar_representation;
        typedef binary_relation range_search2d;

    protected:
        grammar_representation _g;
        range_search2d grid;

    public:
        SelfGrammarIndex(){};

        virtual ~SelfGrammarIndex(){};

        virtual void build(const std::string& );

        //virtual void build(const grammar_representation&, const range_search2d& ) = 0;

        virtual void locate( std::string& , sdsl::bit_vector &) = 0;

        virtual void display(const std::size_t& , const std::size_t&, std::string & );

        virtual void save(std::fstream& );
        virtual void load(std::fstream& );

        virtual unsigned long size_in_bytes() const;

    protected:

        virtual void locate_ch( const char& , sdsl::bit_vector &);

        bool bp_expand_prefix(const grammar_representation::g_long &, std::string&,const size_t &,size_t & pos) const;

        bool bp_expand_suffix(const grammar_representation::g_long &, std::string&,const size_t &,size_t & pos) const;

        bool expand_prefix(const grammar_representation::g_long &, std::string&,const size_t &,size_t & pos) const;

        bool expand_suffix(const grammar_representation::g_long &, std::string&,const size_t &, size_t &) const;

        void expand_grammar_sfx(const size_t &, std::string &, const size_t  &) const ;

        void find_second_occ(long int& , unsigned int &, sdsl::bit_vector& ) const ;

        template <typename  K>
        bool lower_bound(grammar_representation::g_long& lr,grammar_representation::g_long& hr, const K &f) const
        {
            if (lr >= hr || lr+1 == hr )
            {
                if( f(lr) == 0 ) return true;

                if( f(hr) == 0 )
                {
                    lr = hr;
                    return true;
                }
                return false;
            }

            grammar_representation::g_long m = (lr + hr) / 2;

            auto res = f(m);
            if(res < 0)
            {
                /*
                 * If value < value(node) f must return
                 * */
                hr = m-1;
            }
            else
            {
                if(res > 0)
                {
                    /*
                     * If value > value(node) f must return true
                     * */
                    (lr = m);
                }

                else
                {
                    /*
                     * If value == value(node) f must return true
                     * */
                    hr = m;
                }
            }



            return lower_bound(lr, hr, f);
        }

        template <typename  K>
        bool upper_bound(grammar_representation::g_long& lr,grammar_representation::g_long& hr, const K &f) const
        {
            if (lr >= hr || lr+1 == hr )
            {
                if( f(hr) == 0 ) return true;

                if( f(lr) == 0 )
                {
                    hr = lr;
                    return true;
                }
                return false;
            }

            grammar_representation::g_long m= (lr + hr) / 2;

            auto res = f(m);

            if(res < 0)
            {
                /*
                 * If value < value(node) f must return true
                 * */
                hr = m;
            }
            else
            {

                if(res > 0)
                {
                    /*
                     * If value > value(node) f must return true
                     * */
                    (lr = m+1);
                }
                else
                {
                    /*
                    * If value == value(node) f must return true
                    * */
                    lr = m;
                }


            }

            return upper_bound(lr, hr, f);
        }

        int cmp_prefix(const grammar_representation::g_long &,std::string::iterator&, std::string::iterator& );
        int cmp_suffix(const grammar_representation::g_long &,std::string::iterator&, std::string::iterator& );
        int cmp_suffix_grammar(const size_t & ,std::string::iterator&, std::string::iterator& );

        int bp_cmp_prefix(const grammar_representation::g_long &,std::string::iterator&, std::string::iterator& )const;
        int bp_cmp_suffix(const grammar_representation::g_long &,std::string::iterator&, std::string::iterator& )const;
        int bp_cmp_suffix_grammar(const size_t & ,std::string::iterator&, std::string::iterator& )const;


};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H
