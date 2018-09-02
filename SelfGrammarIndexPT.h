//
// Created by alpachec on 20-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H


#include "SelfGrammarIndex.h"
#include "trees/patricia_tree/compact_patricia_tree.h"


class SelfGrammarIndexPT: protected SelfGrammarIndex {



    protected:

        unsigned long sampling;

        m_patricia::compact_patricia_tree sfx_p_tree;
        m_patricia::compact_patricia_tree rules_p_tree;


        void sampling_range_suff(size_t& i, size_t& j,std::vector<size_t >&, std::string&)const;

        void sampling_range_rules(size_t &i, size_t &j,std::vector<size_t >& S, std::string& str) const;

    public:
        SelfGrammarIndexPT() = default;
        ~SelfGrammarIndexPT() = default;

        virtual void build(std::string& ) override ;

        unsigned long size_in_bytes() const override ;
        void build(const grammar_representation&, const range_search2d& ) override ;
        void build(const grammar_representation&, const range_search2d&, const m_patricia::compact_patricia_tree&,const m_patricia::compact_patricia_tree& ) ;

        void locate( std::string& , sdsl::bit_vector &) override ;

        void display(const std::size_t& , const std::size_t&, std::string & ) override ;

        void save(std::fstream& ) override;

        void load(std::fstream& ) override;



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
