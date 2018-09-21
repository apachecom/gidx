//
// Created by alpachec on 20-09-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPTS_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPTS_H



#include "SelfGrammarIndexPT.h"


class SelfGrammarIndexPTS:SelfGrammarIndexPT{

    protected:

    uint8_t sampling;

    public:
        SelfGrammarIndexPTS(const uint8_t &);
        virtual ~SelfGrammarIndexPTS();

        void build(const std::string& ) override ;

        void build(const SelfGrammarIndex::grammar_representation & G,
                   const SelfGrammarIndex::range_search2d & R,
                   const m_patricia::compact_patricia_tree &sfx,
                   const m_patricia::compact_patricia_tree &rules,
                   const unsigned int &);

        unsigned long size_in_bytes() const override ;

        void locate( std::string& , sdsl::bit_vector &) override ;
        void locate2( std::string & , sdsl::bit_vector & );

        void display(const std::size_t& , const std::size_t&, std::string & ) override ;

        void save(std::fstream& ) override;

        void load(std::fstream& ) override;

    protected:

        void sampling_range_suff(size_t& i, size_t& j, std::string::iterator& , std::string::iterator& )const;

        void sampling_range_rules(size_t &i, size_t &j, std::string::iterator& , std::string::iterator& )const;


        size_t _st(const size_t & i)const;





};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPTS_H
