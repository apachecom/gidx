//
// Created by alpachec on 20-08-18.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndexPT.h"


void SelfGrammarIndexPT::build(std::string &text) {

    SelfGrammarIndex::build(text);

    /*
     * Building grammar by repair algorithm
     *
     * */
    std::cout<<"Building grammar by repair algorithm"<<std::endl;


    grammar not_compressed_grammar;
    not_compressed_grammar.buildRepair(text);

    std::cout<<"\t number of rules "<<not_compressed_grammar.n_rules()<<std::endl;
    std::cout<<"\t total size of rules "<<not_compressed_grammar.get_size()<<std::endl;
    std::cout<<"\t size of the representation "<<not_compressed_grammar.size_in_bytes()*1/(1024*1024)<<"(mb)"<<std::endl;

    /////not_compressed_grammar.print(text);


    /*
     * Building compressed grammar
     *
    * */
    std::cout<<"Building compressed grammar"<<std::endl;

 /////   not_compressed_grammar.print(text);
    _g.build(not_compressed_grammar);
    _g.print_size_in_bytes();
    std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()<<std::endl;
    /*
     * Build sufix of grammar
     *
     * */
    std::cout<<"Compute suffix grammar"<<std::endl;

    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
    const auto& gtree = _g.get_parser_tree();
    unsigned long num_sfx = 0;

    for (auto r_begin = not_compressed_grammar.begin(); r_begin != not_compressed_grammar.end(); ++r_begin) {

        rule r = r_begin->second;
        rule::r_long r_id = r_begin->first;
        size_t node = gtree[ _g.select_occ(r_id,1)];

        rule::r_long off = 0;

        for (auto j = r._rule.size()-1; j >= 1 ; --j) {
            off += not_compressed_grammar[r._rule[j]].len();

            size_t  left =  r.r - off + 1;
            size_t  right = r.r;

            size_t tag = gtree.pre_order(gtree.child(node,j+1));
            grammar_sfx.emplace_back(std::make_pair(std::make_pair(left,right),std::make_pair(r._rule[j-1],tag)));
            num_sfx++;
        }
    }
    std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

    /*
     * Sorting suffixes
     *
     * */
    sdsl::int_vector<> SA(text.size(),0);
    sdsl::algorithm::calculate_sa( (unsigned char*)text.c_str(),text.size(),SA);
    sdsl::inv_perm_support<> SA_1(&SA);
    sdsl::lcp_bitcompressed<> LCP;
    sdsl::construct_im(LCP,text.c_str(),sizeof(char));
    sdsl::rmq_succinct_sada<> rmq(&LCP);


   std::sort(grammar_sfx.begin(),grammar_sfx.end(),
              [this,&text,&SA_1,&LCP,&rmq](const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > & a,
                                 const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > &b )->bool{
                  /*
                   * offset of suffix in the text
                   * */
                  ulong a_pos = a.first.first;
                  ulong b_pos = b.first.first;

                  ulong size_a = a.first.second - a.first.first +1;
                  ulong size_b = b.first.second - b.first.first +1;

                  /*
                   * is start at the same position return the shortest
                   * */
                  if(a_pos == b_pos)
                      return size_a < size_b;

                  auto sa_1_a = SA_1[a_pos];
                  auto sa_1_b = SA_1[b_pos];

                  int min= LCP[rmq(std::min(sa_1_a,sa_1_b)+2,std::max(sa_1_a,sa_1_b)+1)];

                  /*
                   * Check if one is prefix of the other
                   * return the shortest
                   * */

                  if(std::min(size_a,size_b) <= min){
                      return size_a < size_b;
                  }

                  /*
                   * then return the lowest lexicographical
                   * */

                  return sa_1_a < sa_1_b;

              });


    /*for (auto && sf  : grammar_sfx) {

        std::string sfs;
        sfs.resize(sf.first.second - sf.first.first + 1);
        std::copy(text.begin()+sf.first.first,text.begin()+sf.first.second+1,sfs.begin());
        std::cout<<sfs<<std::endl;

    }
    */

    /*
     *
     * Building grid for 2d range search first occ
     *
     * */

    std::cout<<"Building grid for 2d range search first occ"<<std::endl;

    std::vector<std::pair<std::pair<range_search2d::bin_long ,range_search2d::bin_long >,range_search2d::bin_long>> grid_points(num_sfx);
    size_t bpair = 0;
    for (auto && s  :grammar_sfx ) {
        grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
        ++bpair;
    }

    grid.build(grid_points.begin(),grid_points.end(),not_compressed_grammar.n_rules(),num_sfx);
    std::cout<<"grid size "<<grid.size_in_bytes()<<std::endl;
    grid.print_size();

    /*
     * Building Sampled Patricia Trees for suffixes.
     *
     * Sampling by log(u)log(log(n))/log(n)
     * where n is the number of symbols in the repair grammar and u is the length of the original text
     * */

    std::cout<<"Building Sampled Patricia Trees for suffixes."<<std::endl;

    sampling = 1;//(unsigned long)(log2(text.size()) * log2(log2(not_compressed_grammar.n_rules()))/log2(not_compressed_grammar.n_rules()));
    sampling = ( sampling != 0 )?sampling:1;
    std::cout<<"\t sampled: "<<sampling<<std::endl;
    std::cout<<"\t numero de sufijos count: "<<num_sfx<<std::endl;
    std::cout<<"\t grammar_sfx.size() "<<grammar_sfx.size()<<std::endl;
    {
        m_patricia::patricia_tree<m_patricia::string_pairs> T;
        unsigned long id = 0;
        for (int i = 0; i < grammar_sfx.size(); i += sampling)
        {
            m_patricia::string_pairs s(text,++id);
            s.set_left(grammar_sfx[i].first.first);
            s.set_right(grammar_sfx[i].first.second);
            T.insert(s);
        }

        sfx_p_tree.build(T);
        std::cout<<"sfx_p_tree size "<<sfx_p_tree.size_in_bytes()<<std::endl;
        sfx_p_tree.print_size_in_bytes();
    }

    /*
     * Building Patricia Trees for rules.
     *
     * */
    std::cout<<"Building Patricia Trees for rules."<<std::endl;
    {
        m_patricia::patricia_tree<m_patricia::rev_string_pairs> T;
        unsigned long id = 0;
        for (auto &&  r: not_compressed_grammar) {
            m_patricia::rev_string_pairs s(text,++id);
            s.set_left(r.second.l);
            s.set_right(r.second.r);
            T.insert(s);
        }
        rules_p_tree.build(T);
        std::cout<<"rules_p_tree size "<<rules_p_tree.size_in_bytes()<<std::endl;
        rules_p_tree.print_size_in_bytes();



    }

}

void SelfGrammarIndexPT::locate( std::string & pattern, sdsl::bit_vector & occ) {

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    ///size_t n_sj = grid.n_columns();

    /*vector<size_t > S (n_sj,1);
    for (size_t j = 0; j < n_sj; ++j)
        S[j] = j+1;*/

    /////std::cout<<"////////////////////////cadena "<<pattern<<" /////////////////////////////////////////////////////////////////"<<std::endl;
    for (size_t i = 1; i <= p_n ; ++i)
    {
        // CREATE PARTITIONS
        /////std::cout<<"////////////////////////PARTITION "<<i<<" /////////////////////////////////////////////////////////////////"<<std::endl;
        std::string p1,p2;
        p1.resize(i);
        p2.resize(p_n-i);

        std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
        std::copy(pattern.begin()+i,pattern.end(),p2.begin());
        std::reverse(p1.begin(),p1.end());
        //BINARY SEARCH OVER THE Frev SORT RULES
        /////// std::cout<<"prefijo rev: "<<p1<<"\t sufijo: "<<p2;
        /////auto start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////
        auto itera = pattern.begin() + i-1;


        //auto rows = rules_trie.matches(p1.begin(),p1.end());

        ////auto start = timer::now();

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern,1);
        sp1.set_left(0);
        sp1.set_right(i-1);

        const auto& rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        const auto& rules_leaf = rules_t.leafrank(node_match_rules);


        auto begin_rule_string = pattern.begin();
        auto end_rule_string = itera;
        auto r = bp_cmp_suffix(rules_leaf,end_rule_string,begin_rule_string);
        if(r != 0 || end_rule_string+1 != begin_rule_string)
            continue;

        size_t p_r1 = rules_t.leafrank(node_match_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;



        /* std::string s_rule;
         s_rule.resize(p1.size());
         size_t pos_sr = 0;
         expand_suffix(rules_leaf,s_rule,sp1.size(),pos_sr);
         std::reverse(s_rule.begin(),s_rule.end());

         unsigned long p = 0;
         while(p < sp1.size() && s_rule[p] == sp1[p]) ++p;

         if(p == 0 && !sp1.empty()) continue;

         auto node_locus_rules = rules_p_tree.node_locus(sp1,p);
         std::pair<size_t , size_t > rows(0,0);
         rows.first = rules_t.leafrank(node_locus_rules);
         rows.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;
         ////auto stop = timer::now();
         ////std::cout<<"\t\trules PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;
         std::cout<<" rule PT search"<<std::endl;
         /////auto start = timer::now();

         //////////////////////////////////////////////////////////////
         size_t p_r1 = rows.first,p_r2 = rows.second;
         if(p_r1 == 0 && p_r2 == 0)continue;
         ////std::cout<<"rows_1("<<p_r1<<") \t rows_2)("<<p_r2<<")\n";
 */

        //// start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */

        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

         auto begin_sfx_string = itera+1;
         auto end_sfx_string = pattern.end() ;
         r = bp_cmp_suffix_grammar(suff_leaf,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        size_t p_c1 = suff_t.leafrank(node_match_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;

        /*std::string s_sufx;
        expand_grammar_sfx((rules_leaf-1)*sampling + 1,s_sufx,p2.size());

        p = 0;
        while(p < sp2.size() && s_sufx[p] == sp2[p]) ++p;
        if(p == 0 && !sp1.empty()) continue;
        */
  /*      auto node_locus_suff = sfx_p_tree.node_locus(sp2,p);
        std::pair<size_t , size_t > cols(0,0);
        cols.first = rules_t.leafrank(node_locus_suff);
        cols.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;


        size_t p_c1 = cols.first,p_c2 = cols.second;
        if(p_c1 == 0 && p_c2 == 0)continue;
*/
      ////  sampling_range_suff(p_c1,p_c2,S,p2);

        ////stop = timer::now();
        ////std::cout<<"\t\tbinary search logn sampling "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;

        //std::cout<<" sfx PT search"<<std::endl;


        // CHECK THE RANGE

        ////start = timer::now();

        /////////////////////////////////////////////////////////////////////////////////////////
  /*      std::string p1s , p2s;
        expand_grammar_sfx(p_r1,p1s,p1.length());
        ///std::cout<<"rows_1: "<<p1s<<std::endl;
        if(p1 != p1s) continue;
        expand_grammar_sfx(p_r2,p2s,p1.length());
        ///std::cout<<"rows_2: "<<p2s<<std::endl;
        if(p1 != p2s) continue;
        // CHECK THE RANGE

        // CHECK THE RANGE
        p1s =""; p2s="";
        expand_grammar_sfx(p_c1,p1s,p2.length());
        ///std::cout<<"cols_1: "<<p1s<<std::endl;
        if(p2 != p1s) continue;
        expand_grammar_sfx(p_c2,p2s,p2.length());
        ///std::cout<<"cols_2: "<<p2s<<std::endl;
        if(p2 != p2s) continue;
    */    /////////////////////////////////////////////////////////////////////////////////////////

        ////stop = timer::now();
        ////std::cout<<"\t\tcheck ranges time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;
        // CHECK THE RANGE

        std::cout<<" END THE PATRICIA TREE"<<std::endl;

        std::vector< std::pair<size_t,size_t> > pairs;
        ////start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        binary_relation::bin_long  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        grid.range(x1,x2,y1,y2,pairs);



        ///std::cout<<"pairs size "<<pairs.size()<<std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////
        ////stop = timer::now();
        ////std::cout<<"\t\t2d search binary relation time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;


        long len = itera-pattern.begin() +1;
        const auto& g_tree = _g.get_parser_tree();
        ////start = timer::now();
        for (auto &pair : pairs) {


            size_t p = grid.labels(pair.first, pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long int  l = (- len + pos_p) - _g.offsetText(parent);


            ///start = timer::now();
            find_second_occ(l,parent,occ);
            //// stop = timer::now();
            ///std::cout<<"\t\tsearch_grammar_tree_second_occ time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;

            /*std::cout<<"occ"<<std::endl;
            for (auto &&i  :occ ) {
                std::cout<<i<<" ";
            }
            std::cout<<std::endl;*/

            /////////////////////////////////////////////////////////////////////////////////////////


        }

    }



}

void SelfGrammarIndexPT::save(std::fstream & f_out) {
    SelfGrammarIndex::save(f_out);
    sfx_p_tree.save(f_out);
    rules_p_tree.save(f_out);
}

void SelfGrammarIndexPT::load(std::fstream & f_in) {
    SelfGrammarIndex::load(f_in);
    sfx_p_tree.load(f_in);
    rules_p_tree.load(f_in);

}

void SelfGrammarIndexPT::sampling_range_suff(size_t &i, size_t &j,std::vector<size_t >& S,const std::string& str) const {

    if(i != j){

        if(i > 1){

            auto r1 = std::lower_bound(S.begin(),S.end(),str,[&i,this](const size_t  &a, const std::string &s){
                size_t  n_s = s.length();
                std::string sufx;
                size_t id_sufix = (i-2)*sampling + a;
                expand_grammar_sfx(id_sufix,sufx,n_s);
                if(std::strcmp(sufx.c_str(),s.c_str()) < 0) // s > a
                    return 1;
                return 0;
            });

            i = ( r1 != S.end() ) ? *r1 : 1;

        }

        auto sfx_n = grid.n_columns();
        if( j <= sfx_n ){

            size_t end = ( j + sampling <= sfx_n)? sampling:(sfx_n-j+1);

            auto r2 = std::upper_bound(S.begin(),S.begin()+end,str,[&j,this](const std::string &s,const size_t &a){
                size_t n_s = s.length();
                std::string sufx;
                size_t id_sufix = (j-1)*sampling + a;
                expand_grammar_sfx(id_sufix,sufx,n_s);
                if(s < sufx)
                    return 1;
                return 0;
            });

            j  = *(--r2);
        }


    }else{

        auto sfx_n = grid.n_columns();

        std::vector<uint> X(2*sampling);
        for(uint i = 0 ; i < X.size(); ++i){
            X[i] = i+1;
        }

        uint low = (i-1)*sampling, r = ( j + sampling <= sfx_n)? sampling:(sfx_n-j+1);

        if(i > 1)
            low = (i-2)*sampling;

        auto r1 = std::lower_bound(X.begin(),X.begin()+r,str,[&low,this](const size_t  &a, const std::string &s){
            size_t  n_s = s.length();
            std::string sufx;
            size_t id_sufix = low + a;
            expand_grammar_sfx(id_sufix,sufx,n_s);
            if(std::strcmp(sufx.c_str(),s.c_str()) < 0) // s > a
                return 1;
            return 0;
        });

        i =  ( r1 != X.begin()+r ) ? *r1 : 1;


        auto r2 = std::upper_bound(X.begin(),X.begin()+r,str,[&low,this](const std::string &s,const size_t &a){
            size_t n_s = s.length();
            std::string sufx;
            size_t id_sufix = low + a;
            expand_grammar_sfx(id_sufix,sufx,n_s);
            if(s < sufx)
                return 1;
            return 0;
        });

        j = *(--r2);
    }


}

unsigned long SelfGrammarIndexPT::size_in_bytes() const {
    return SelfGrammarIndex::size_in_bytes() + sfx_p_tree.size_in_bytes() + rules_p_tree.size_in_bytes();
}

void SelfGrammarIndexPT::display(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display( i,j,s);
}

void
SelfGrammarIndexPT::build(const SelfGrammarIndex::grammar_representation & G, const SelfGrammarIndex::range_search2d & R,
        const m_patricia::compact_patricia_tree &sfx, const m_patricia::compact_patricia_tree &rules) {
    _g = G;
    grid = R;
    sfx_p_tree = sfx;
    rules_p_tree = rules;
}

void
SelfGrammarIndexPT::build(const SelfGrammarIndex::grammar_representation &, const SelfGrammarIndex::range_search2d &) {

}
