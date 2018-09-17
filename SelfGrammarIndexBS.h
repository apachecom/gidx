//
// Created by alpachec on 19-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H


#include "SelfGrammarIndex.h"

class SelfGrammarIndexBS: protected SelfGrammarIndex {


public:

    SelfGrammarIndexBS()= default;
    ~SelfGrammarIndexBS() override = default;


    void build(const std::string& ) override;

    void build(const grammar_representation&, const range_search2d& );

    void locate( std::string& , sdsl::bit_vector &) override;
    void display(const std::size_t& , const std::size_t&, std::string & ) override ;


    void save(std::fstream& f)
    {
        SelfGrammarIndex::save(f);
    }
    void load(std::fstream& f)
    {
        SelfGrammarIndex::load(f);
    }


protected:



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
