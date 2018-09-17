//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_GRAMMAR_H
#define IMPROVED_GRAMMAR_INDEX_GRAMMAR_H


#include <vector>
#include <map>
#include <assert.h>
#include "repair/RePair.h"
#include <io.hpp>


struct rule{


    typedef unsigned int r_long;
    r_long id;
    bool terminal;
    /*
     * Rule production
     * */
    std::vector<rule::r_long > _rule;
    /*
     * offset in the text
     * */
    rule::r_long l,r;

    rule():id(0),terminal(false),l(0),r(0){}
    rule(const r_long & i, const bool& t):id(i),terminal(t),l(0),r(0){}
    rule& operator=(const rule& R)
    {
        id = R.id;
        terminal = R.terminal;
        _rule = R._rule;
        l = R.l;
        r = R.r;
    }
    rule::r_long size_in_bytes()const
    {
        return sizeof(rule::r_long)*3 + 1 + _rule.size()*sizeof(rule::r_long);
    }

    rule::r_long len() const{ return r - l + 1; }

    void load(std::fstream& f)
    {
        sdsl::load(id,f);
        sdsl::load(terminal,f);
        sdsl::load(r,f);
        sdsl::load(l,f);
        rule::r_long n;
        sdsl::load(n,f);
        _rule.clear();
        _rule.resize(n);
        for (int i = 0; i < n ; ++i) {
            rule::r_long item;
            sdsl::load(item,f);
            _rule[i] = item;
        }
    }
    void save (std::fstream& f)
    {
        sdsl::serialize(id,f);
        sdsl::serialize(terminal,f);
        sdsl::serialize(r,f);
        sdsl::serialize(l,f);
        rule::r_long ns = _rule.size();
        sdsl::serialize(ns,f);

        for (auto &&  item: _rule) {
            sdsl::serialize(item,f);
        }
    }


};

class grammar {

    public:

        typedef std::map<rule::r_long ,rule>::iterator grammar_iterator;

    protected:
        std::map<rule::r_long ,rule> _grammar;

        rule::r_long initial_rule;

        rule::r_long _size;

        std::map<unsigned int,unsigned char> alp;

        void preprocess(const std::string &);

        void replace(const rule::r_long& , std::map<rule::r_long ,rule::r_long > &, std::set<rule::r_long> &);

        void compute_offset_text(const rule::r_long& ,  rule::r_long&, std::set<rule::r_long>&);



    public:


        grammar();

        ~grammar();

        void buildRepair(const std::string&);

        grammar_iterator begin();

        grammar_iterator end();

        const rule& operator[](const rule::r_long&);

        const std::map<unsigned int,unsigned char>& get_map() const;


        template< typename F >
        void dfs(const rule::r_long& X, const F &f ){

                assert(_grammar.find(X) != _grammar.end() );
                rule r = _grammar[X];

                if(!f(r)) return;

                for (int i = 0; i < r._rule.size() ; ++i) {
                    dfs(r._rule[i],f);
                }
            }

        const rule::r_long& get_size();

        rule::r_long n_rules();

        rule::r_long size_in_bytes();

        const rule::r_long& get_initial_rule();

        rule::r_long text_size();

        void print(const std::string& );

        rule::r_long num_terminals()const ;

        void save(std::fstream& f);

        void load(std::fstream& f);

        void write_text( fstream & out);
};


#endif //IMPROVED_GRAMMAR_INDEX_GRAMMAR_H
