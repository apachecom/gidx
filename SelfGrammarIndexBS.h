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
    void locate( std::string& , std::vector<uint> &) override;
    void locate2( std::string& , sdsl::bit_vector &) ;
    void display(const std::size_t& , const std::size_t&, std::string & ) override ;


    void save(std::fstream& f)
    {
        SelfGrammarIndex::save(f);
    }
    void load(std::fstream& f)
    {
        SelfGrammarIndex::load(f);
    }
    unsigned long size_in_bytes() const override ;

    const grammar_representation& get_grammar() const  { return SelfGrammarIndex::get_grammar();}
    const range_search2d & get_grid() const  { return SelfGrammarIndex::get_grid();}
    bool bp_expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const{
        SelfGrammarIndex::bp_expand_prefix(X,s,l,pos);
    }
    bool expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const{
        SelfGrammarIndex::expand_prefix(X,s,l,pos);
    }

    int cmp_prefix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const{
        return SelfGrammarIndex::cmp_prefix(X,i,j);
    }
    int cmp_suffix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const{
        return SelfGrammarIndex::cmp_suffix(X,i,j);
    }


protected:



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
