//
// Created by inspironXV on 8/16/2018.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndex.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;



void SelfGrammarIndex::build(const std::string& text)
{





}

void SelfGrammarIndex::save(std::fstream &fstream1) {

    _g.save(fstream1);
    grid.save(fstream1);
}

void SelfGrammarIndex::load(std::fstream &fin) {

    _g.load(fin);
    grid.load(fin);


}

bool SelfGrammarIndex::expand_prefix(const grammar_representation::g_long & X, std::string & s, const size_t & l,size_t & pos )const
{

    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf <= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X_i = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X_i))
        {
            unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
            s[pos]= a_th;
            if(l == ++pos) return true;
            current_leaf++;

            while(current_leaf > last_leaf && !Q.empty()){
                current_leaf = Q.front().second.first+1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }


        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X_i,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }
    return true;

}

bool SelfGrammarIndex::expand_suffix(const grammar_representation::g_long  &X_i, std::string &s, const size_t & l,size_t & pos ) const
{

    assert(X_i > 0 && X_i < _g.n_rules());


    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }

    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_right_trie();
    const auto& dfuds_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();
    /*
     * Find the right most path for X_i in left_trie
     *
     * */

    /*
     * Finding the preorder of the node with tag X_i in right most path trie
     * */
    size_t  pre_trmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in right most path trie
     * */
    size_t  node_xi_right_path = dfuds_rep[pre_trmp_xi];

    /*
     * Finding path to the root of the right most path trie
     * */
    std::deque<size_t > S;
    S.emplace_front(node_xi_right_path);
    size_t current = node_xi_right_path;


    while(dfuds_rep.root() && dfuds_rep.parent(current) != dfuds_rep.root()){
        current = dfuds_rep.parent(current); // DFUDS DO NOT SUPPORT DEPTH OP
        S.emplace_front(current);
    }

    /*
     *  root's child ancestor
     *
     * */
    size_t  X_a = seq[dfuds_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a);
        s[pos] = a_th;

        if(l == ++pos)
            return true;
        S.pop_front();
    }


    while(!S.empty()){

        size_t  top = S.front();
        size_t  p = dfuds_rep.pre_order(top);
        size_t  X = seq[p-1];

        size_t  sym = _g.select_occ(X,1);


        size_t  u = Tg[sym];
        size_t  n_ch = Tg.children(u);

        size_t  begin = (top == current)?n_ch:n_ch-1;

        for (size_t  i = begin ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            if( expand_suffix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }

        S.pop_front();
    }

    return false;

}

void SelfGrammarIndex::expand_grammar_sfx(const size_t & sfx, std::string & s, const size_t & l) const
{


    if(l == 0)
    {
        s = "";
        return;
    }
    s.resize(l);

    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];
    size_t pos = 0;
    expand_prefix(X,s,l,pos);



    if(pos < l){

        auto node = parser_tree[sfx_preorder];
        auto parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;

        while(pos < l && child_r <= n ){
            //std::string s1;
            size_t r = parser_tree.child(parent,child_r);
            r = parser_tree.pre_order(r);
            r = _g[r];
            expand_prefix(r,s,l,pos);
            child_r ++;
        }

    }
}
void SelfGrammarIndex::find_second_occ(long int & offset, unsigned int & node, std::vector<uint> & occ) const{

    const auto& Tg = _g.get_parser_tree();
    std::deque<std::pair< uint, long int >> S;

    {
        size_t pre = Tg.pre_order(node);
        size_t Xi = _g[pre];
        size_t n_s_occ = _g.n_occ(Xi);
        for (size_t i = 1; i <= n_s_occ; ++i)
        {
            size_t pre_parent = _g.select_occ(Xi,i);
            S.emplace_back(pre_parent,offset);
        }
    }

    ////uint steps = 0;
    while(!S.empty())
    {
        ///std::pair<size_t , long int > front_pair = S.front();

        if(S.front().first == 1)
        {
            occ.push_back((uint)(S.front().second));
        }
        else
        {
            auto node = Tg[S.front().first];
            size_t parent = Tg.parent(node);
            size_t pre_parent = Tg.pre_order(parent);
            size_t Xi = _g[pre_parent];
            size_t n_s_occ = _g.n_occ(Xi);
            long int p_offset = S.front().second + _g.offsetText(node) - _g.offsetText(parent);
            for (size_t i = 1; i <= n_s_occ; ++i)
            {
                size_t pre_parent = _g.select_occ(Xi,i);
                S.emplace_back(pre_parent,p_offset);
            }
        }

        S.pop_front();
    }

}
void SelfGrammarIndex::find_second_occ(long int & offset, unsigned int & node, sdsl::bit_vector & occ) const{

    const auto& Tg = _g.get_parser_tree();
    std::deque<std::pair< uint, long int >> S;

    {
        size_t pre = Tg.pre_order(node);
        size_t Xi = _g[pre];
        size_t n_s_occ = _g.n_occ(Xi);
        for (size_t i = 1; i <= n_s_occ; ++i)
        {
            size_t pre_parent = _g.select_occ(Xi,i);
            S.emplace_back(pre_parent,offset);
        }
    }

    ////uint steps = 0;
    while(!S.empty())
    {
        ///std::pair<size_t , long int > front_pair = S.front();

        if(S.front().first == 1)
        {
            occ[S.front().second ] = true;
        }
        else
        {
            auto node = Tg[S.front().first];
            size_t parent = Tg.parent(node);
            size_t pre_parent = Tg.pre_order(parent);
            size_t Xi = _g[pre_parent];
            size_t n_s_occ = _g.n_occ(Xi);
            long int p_offset = S.front().second + _g.offsetText(node) - _g.offsetText(parent);
            for (size_t i = 1; i <= n_s_occ; ++i)
            {
                size_t pre_parent = _g.select_occ(Xi,i);
                S.emplace_back(pre_parent,p_offset);
            }
        }

        S.pop_front();
    }

}

void SelfGrammarIndex::display(const std::size_t & i, const std::size_t & j, std::string & str) {

    //xassert(i >= 0 && j >= 0 && i <= j && i < _g.get_size_text() && j < _g.get_size_text()); // border assert

    // FIND THE NODE WHERE i OCCURS

    const auto& Tg = _g.get_parser_tree();
    long long int p = i;

    std::stack<dfuds::dfuds_tree::dfuds_long> s_path;
    dfuds::dfuds_tree::dfuds_long root = Tg.root();


  // auto start = timer::now();

    auto current_node = root;
    bool notend = true;

    while(notend)
    {
        s_path.push(current_node);
        if(Tg.isleaf(current_node))
        {
            auto X_j = _g[Tg.pre_order(current_node)];
            if(_g.isTerminal(X_j)){ // if is a rule X_j -> a
                break;
            }
            p -= _g.offsetText(current_node);
            auto occ_p = _g.select_occ(X_j,1);
            current_node = Tg[occ_p];
            p += _g.offsetText(current_node);
        }

        dfuds::dfuds_tree::dfuds_long ch = Tg.children(current_node);
        dfuds::dfuds_tree::dfuds_long ls = 1, hs = ch;


       /* std::vector<uint32_t > chs(ch);
        for (int k = 0; k < ch; ++k) {
            chs[k] = k+1;
        }
        uint child,pos_m;
        bool found = false;

        auto ppp = std::lower_bound(chs.begin(),chs.end(),p,[&pos_m,&current_node,&child,&found,this, &Tg](const uint32_t& a, const  long long int &p){
            child = Tg.child(current_node,a);
            pos_m = _g.offsetText(child);
            if(pos_m == p) found = true;
            return pos_m < p;
        });

        if(!found)
            current_node = child;
        else
            current_node = Tg.child(current_node,*ppp);*/

        dfuds::dfuds_tree::dfuds_long  l = Tg.find_child(current_node,ls,hs,[&p,this](const dfuds::dfuds_tree::dfuds_long &child)->bool{
            size_t pos_m = _g.offsetText(child);
            return p < pos_m;
        });

        current_node = Tg.child(current_node,l);

    }


/*
    Tg.path(root , [&p,&s_path,this,&Tg](dfuds::dfuds_tree::dfuds_long &node, dfuds::dfuds_tree::dfuds_long &next_node)->bool{

        s_path.push(node);


        if(Tg.isleaf(node)){

            // If node is a second occurence
            auto X_j = _g[Tg.pre_order(node)];
            if(_g.isTerminal(X_j)){ // if is a rule X_j -> a
                return false;
            }

            p -= _g.offsetText(node);
            auto occ_p = _g.select_occ(X_j,1);
            node = Tg[occ_p];
            //s_path.pop();
            //s_path.push(node);
            //return true;
            p += _g.offsetText(node);
        }

        dfuds::dfuds_tree::dfuds_long ch = Tg.children(node);
        dfuds::dfuds_tree::dfuds_long ls = 1, hs = ch;


//
//        std::vector<uint32_t > chs(ch);
//        for (int k = 0; k < ch; ++k) {
//            chs[k] = k+1;
//        }
//        uint child,pos_m;
//        bool found = false;
//
//        auto ppp = std::lower_bound(chs.begin(),chs.end(),p,[&pos_m,&next_node,&child,&found,this, &Tg,&node](const uint32_t& a, const  long long int &p){
//            child = Tg.child(node,a);
//            pos_m = _g.offsetText(child);
//            if(pos_m == p) found = true;
//            return pos_m < p;
//        });
//
//        if(!found)
//            next_node = child;
//        else
//            next_node = Tg.child(node,*ppp);
//
//
//
//        return true;



        dfuds::dfuds_tree::dfuds_long  l = Tg.find_child(node,ls,hs,[&p,this](const dfuds::dfuds_tree::dfuds_long &child)->bool{
            size_t pos_m = _g.offsetText(child);
            return p < pos_m;
        });

        next_node = Tg.child(node,l);


 //       stop = timer::now();
 //       std::cout<<"childs: "<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;
        return true;
    });
*/

    /*auto stop = timer::now();
    std::cout<<"path: "<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;
*/

   // start = timer::now();
    long long int off = j-i+1;
    str.resize(off);
    size_t pos = 0;
    expand_prefix(_g[Tg.pre_order(s_path.top())],str,(size_t)off,pos);

    while(!s_path.empty() && pos < off  ){

        size_t node = s_path.top();
        size_t parent = Tg.parent(node);
        size_t r = Tg.childrank(node);
        size_t n_ch = Tg.children(parent);

        for (auto k = r+1; k <= n_ch && pos < off ; ++k)
        {
            node = Tg.pre_order(Tg.child(parent,k));
            expand_prefix(_g[node],str,(size_t)off,pos);
        }
        s_path.pop();
    }
//    stop = timer::now();
 //   std::cout<<"expand: "<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;



  //  start = timer::now();
    /*long long int off = j-i+1;
    str.resize(off);
    size_t pos = 0;
    //expand_prefix(_g[Tg.pre_order(s_path.top())],str,(size_t)off,pos);
    expand_prefix2(s_path.top(),str,(size_t)off,pos);

    while(!s_path.empty() && pos < off ){

        size_t node = s_path.top();
        size_t parent = Tg.parent(node);
        auto lnode = Tg.lchild(parent);
        auto current = Tg.nsibling(node);
        while(current != lnode && pos < off)
        {
            node = Tg.pre_order(current);
            expand_prefix2(node,str,(size_t)off,pos);
            //expand_prefix(_g[node],str,(size_t)off,pos);
            current = Tg.nsibling(current);
        }

        if(current == lnode && pos < off)
        {
            node = Tg.pre_order(current);
            //expand_prefix(_g[node],str,(size_t)off,pos);
            expand_prefix2(node,str,(size_t)off,pos);
        }


        s_path.pop();
    }*/
   /* stop = timer::now();
    std::cout<<"expand: "<<duration_cast<nanoseconds>(stop - start).count()<<std::endl;
*/

}

unsigned long SelfGrammarIndex::size_in_bytes() const {
    return _g.size_in_bytes() + grid.size_in_bytes();
}

int SelfGrammarIndex::cmp_prefix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const{

    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X_i,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf <= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X_i = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X_i))
        {
            unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
            if(a_th < (unsigned char)(*itera)) return 1;
            if(a_th > (unsigned char)(*itera)) return -1;
            ++itera;
            if(itera == end) return 0;
            current_leaf++;

            while(current_leaf > last_leaf && !Q.empty())
            {
                current_leaf = Q.front().second.first+1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }
        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X_i,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }
    return 0;

}

int SelfGrammarIndex::cmp_suffix_grammar(const size_t & sfx, std::string::iterator & iterator1, std::string::iterator & iterator2)
{

    if(iterator1 == iterator2)
    {
        return 1;
    }


    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];

    auto r = cmp_prefix(X,iterator1,iterator2);

    if(r == 0)
    {
        auto node = parser_tree[sfx_preorder];
        auto parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;

        while(r == 0 && iterator1!=iterator2 && child_r <= n ){
            size_t rnode = parser_tree.child(parent,child_r);
            rnode = parser_tree.pre_order(rnode);
            auto Y = _g[rnode];
            r = cmp_prefix(Y,iterator1,iterator2);
            child_r ++;
        }

        if(r == 0 && child_r > n && iterator1 != iterator2) return  1; // the sufix is prefix of the string
    }

    return r;
}

int SelfGrammarIndex::cmp_suffix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const {

    assert(X_i > 0 && X_i < _g.n_rules());



    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X_i,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;
    swap(current_leaf,last_leaf);

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf >= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X_i = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X_i))
        {
            unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
            if(a_th < (unsigned char)(*itera)) return 1;
            if(a_th > (unsigned char)(*itera)) return -1;
            --itera;
            if(itera == end-1) return 0;
            --current_leaf;
            while(current_leaf < last_leaf && !Q.empty()){
                current_leaf = Q.front().second.first-1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }

        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X_i,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
            swap(current_leaf,last_leaf);
        }
    }
    return 0;

}

bool SelfGrammarIndex::bp_expand_prefix(const compressed_grammar::g_long &X_i,std::string & s, const size_t & l,size_t & pos) const
{

    //assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();


    /*
    * Find the left most path for X_i in left_trie
    *
    * */


    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
   /*
    * root's child ancestor
    * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet
        s[pos]= a_th;

        if(l == ++pos)
            return true;
    }


    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);

        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = 2 ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            if( bp_expand_prefix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }

        --depht_node_xi;
    }


    return false;
}

bool SelfGrammarIndex::bp_expand_suffix(const compressed_grammar::g_long &X_i,std::string & s, const size_t & l,size_t & pos) const
{
    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }

    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_right_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();
    /*
    * Find the left most path for X_i in left_trie
    *
    * */

    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet
        s[pos]= a_th;

        if(l == ++pos)
            return true;
    }


    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = n_ch-1 ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            if( bp_expand_suffix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }

        --depht_node_xi;
    }


    return false;

}

int
SelfGrammarIndex::bp_cmp_prefix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const {
    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        ++itera;
        ///if(itera == end) return 0;
        return 0;
    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();

    /*
    * Find the left most path for X_i in left_trie
    *
    * */


    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return -1;
        ++itera;
        if(itera == end) return 0;
    }

    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = 2 ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            auto r = bp_cmp_prefix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end)
                return r;
        }

        --depht_node_xi;
    }


    return 0;
}

int
SelfGrammarIndex::bp_cmp_suffix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const{

    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        --itera;
        return 0;
        ////if(itera == end) return 0;

    }

    const auto &Tg = _g.get_parser_tree();
    const auto &tree = _g.get_right_trie();
    const auto &bp_rep = tree.get_tree();
    const auto &seq = tree.get_seq();
    const auto &inv_seq = tree.get_seq_inv();
    /*
    * Find the left most path for X_i in left_trie
    *
    * */

    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        --itera;
            if(itera == end-1) return 0;
    }



    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = n_ch-1 ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            auto r = bp_cmp_suffix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end-1 )
                return r;
        }

        --depht_node_xi;
    }


    return 0;

}

int SelfGrammarIndex::bp_cmp_suffix_grammar(const size_t & sfx, std::string::iterator & iterator1, std::string::iterator & iterator2) const {

    if(iterator1 == iterator2)
    {
        return 1;
    }


    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];

    auto r = bp_cmp_prefix(X,iterator1,iterator2);

    if(r == 0)
    {
        auto node = parser_tree[sfx_preorder];
        auto parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;

        while(r == 0 && iterator1!=iterator2 && child_r <= n ){
            size_t rnode = parser_tree.child(parent,child_r);
            rnode = parser_tree.pre_order(rnode);
            auto Y = _g[rnode];
            r = bp_cmp_prefix(Y,iterator1,iterator2);
            child_r ++;
        }

        if(r == 0 && child_r > n && iterator1 != iterator2) return  1; // the sufix is prefix of the string
    }

    return r;
}

void SelfGrammarIndex::locate_ch(const char & ch, std::vector<uint> & occ) const{

    const auto& alp = _g.get_alp();

    auto p = std::find(alp.begin(),alp.end(),ch);

    if(p == alp.end()) return;

    unsigned int rk = p - alp.begin()+1;
    auto X_a = _g.terminal_rule(rk);

    const auto& Tg = _g.get_parser_tree();
    size_t n_s_occ = _g.n_occ(X_a);
    for (size_t i = 1; i <= n_s_occ; ++i)
    {
        size_t node_occ_pre = _g.select_occ(X_a,i);
        size_t current = Tg[node_occ_pre];
        auto current_parent = Tg.parent(current);
        long int p_offset = _g.offsetText(current) - _g.offsetText(current_parent) ;
        find_second_occ(p_offset,current_parent,occ);

    }

    return;
}


void SelfGrammarIndex::locate_ch(const char & ch, sdsl::bit_vector & occ) const {

    const auto& alp = _g.get_alp();

    auto p = std::find(alp.begin(),alp.end(),ch);

    if(p == alp.end()) return;

    unsigned int rk = p - alp.begin()+1;
    auto X_a = _g.terminal_rule(rk);

    const auto& Tg = _g.get_parser_tree();
    size_t n_s_occ = _g.n_occ(X_a);
    for (size_t i = 1; i <= n_s_occ; ++i)
    {
        size_t node_occ_pre = _g.select_occ(X_a,i);
        size_t current = Tg[node_occ_pre];
        auto current_parent = Tg.parent(current);
        long int p_offset = _g.offsetText(current) - _g.offsetText(current_parent) ;
        find_second_occ(p_offset,current_parent,occ);

    }

    return;
}

bool SelfGrammarIndex::expand_prefix2(const size_t & node, std::string & s, const size_t & l, size_t &pos) const {

    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;



    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;


    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf <= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X_i = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X_i))
        {
            unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
            s[pos]= a_th;
            if(l == ++pos) return true;
            current_leaf++;

            if(current_leaf > last_leaf && !Q.empty())
            {
                current_leaf = Q.front().second.first+1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }
        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X_i,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }
    return true;

}
