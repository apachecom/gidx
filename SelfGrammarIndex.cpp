//
// Created by inspironXV on 8/16/2018.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndex.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;



void SelfGrammarIndex::build(std::string& text)
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

bool SelfGrammarIndex::expand_prefix(const grammar_representation::g_long & X_i, std::string & s, const size_t & l,size_t & pos )const
{

    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& dfuds_rep = tree.get_tree();
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
    size_t  node_xi_left_path = dfuds_rep[pre_tlmp_xi];

    /*
     * Finding path to the root of the left most path trie
     * */
    std::deque<size_t > S;
    size_t last = node_xi_left_path;
    S.emplace_front(last);
    size_t current = dfuds_rep.parent(last);


    while(current != dfuds_rep.root())
    {
        last = current;
        S.emplace_front(last);
        current = dfuds_rep.parent(last); // DFUDS DO NOT SUPPORT DEPTH OP

    }
    /*
     *  root's child ancestor
     * */
    size_t  X_a = seq[dfuds_rep.pre_order(last)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet
        s[pos]= a_th;

        if(l == ++pos)
            return true;
        S.pop_front();
    }


    while(!S.empty())
    {

        size_t  top = S.front();
        size_t  preoreder_trie = dfuds_rep.pre_order(top);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        size_t  begin = (top == current)?1:2;

        for (size_t  i = begin ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            if( expand_prefix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }

        S.pop_front();

    }


    return false;

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

void SelfGrammarIndex::find_second_occ(long int & offset, unsigned int & node, sdsl::bit_vector & occ) const{

    const auto& Tg = _g.get_parser_tree();
    std::deque<std::pair< uint, long int >> S;

    {
        size_t pre = Tg.pre_order(node);
        size_t Xi = _g[pre];
        size_t n_s_occ = _g.n_occ(Xi);
        for (size_t i = 1; i <= n_s_occ; ++i)
        {
            /*size_t node_occ_pre = _g.select_occ(Xi,i);
            size_t current = Tg[node_occ_pre];
            auto current_parent = Tg.parent(current);
            long int p_offset = offset + _g.offsetText(current) - _g.offsetText(current_parent) ;
            */size_t pre_parent = _g.select_occ(Xi,i);
            S.emplace_back(pre_parent,offset);
        }
    }

/*
    {
        uint pre_node = Tg.pre_order(node);
        uint Xi = _g[pre_node];
        uint n_s_occ = _g.n_occ(Xi);
        if(pre_node == 1)
        {
            occ[offset] = true;
            return;
        }
        for (uint  i = 1; i <= n_s_occ; ++i)
        {
            size_t node_occ_pre = _g.select_occ(Xi,i);
            size_t current = Tg[node_occ_pre];
            auto current_parent = Tg.parent(current);
            long int p_offset = offset + _g.offsetText(current) - _g.offsetText(current_parent) ;
            S.emplace_back(node_occ_pre,p_offset);
        }



        /*size_t current_parent = Tg.parent(node);
        long int p_offset = offset + _g.offsetText(node) - _g.offsetText(current_parent);

        if(pre_node == 1)
        {
            occ[p_offset] = true;
            return;
        }
        else
        {
            S.emplace_back(pre_node,p_offset);
        }

        uint i = 1,node_occ_pre = 0;

        do{

            node_occ_pre = _g.select_occ(Xi,++i);
            size_t current = Tg[node_occ_pre];
            current_parent = Tg.parent(current);
            long int p_offset = offset + _g.offsetText(current) - _g.offsetText(current_parent) ;

            if(node_occ_pre == 1)
            {
                occ[p_offset] = true;
            }
            else
            {
                S.emplace_back(node_occ_pre,p_offset);
            }

        }while(!_g.isLastOcc(node_occ_pre));
*/
/*
        for (uint  i = 1; i < n_s_occ; ++i)
        {
            size_t node_occ_pre = _g.select_occ(Xi,i+1);
            size_t current = Tg[node_occ_pre];
            current_parent = Tg.parent(current);
            long int p_offset = offset + _g.offsetText(current) - _g.offsetText(current_parent) ;

            if(node_occ_pre == 1)
            {
                occ[p_offset] = true;
            }
            else
            {
                S.emplace_back(node_occ_pre,p_offset);
            }

        }

*//*
    }
*/
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



           /* uint i = 1,node_occ_pre = 0;

            do{
                node_occ_pre = _g.select_occ(Xi,++i);
                current = Tg[node_occ_pre];
                size_t current_parent = Tg.parent(current);
                long int p_offset = S.front().second + _g.offsetText(current) - _g.offsetText(current_parent);

                if(node_occ_pre == 1)
                {
                    occ[p_offset] = true;
                }
                else
                {
                    S.emplace_back(node_occ_pre,p_offset);
                }

            }while(!_g.isLastOcc(node_occ_pre));*/
        }
/*
        for (size_t i = 1; i < n_s_occ; ++i)
        {
            size_t node_occ_pre = _g.select_occ(Xi,i+1);
            current = Tg[node_occ_pre];
            size_t current_parent = Tg.parent(current);
            long int p_offset = S.front().second + _g.offsetText(current) - _g.offsetText(current_parent);

            if(node_occ_pre == 1)
            {
                occ[p_offset] = true;
            }
            else
            {
                S.emplace_back(node_occ_pre,p_offset);
            }
        }
*/
        S.pop_front();

    }


    ///std::cout<<"steps: "<<count<<std::endl;
    //////std::cout<<steps<<"**************"<<sdsl::bit_vector::rank_1_type(&occ).rank(occ.size())<<std::endl;

      /////auto stop = timer::now();
    ////  std::cout<<"find_second_occ \t\t"<<duration_cast<microseconds>(stop - start).count()<<std::endl;

}

void SelfGrammarIndex::display(const std::size_t & i, const std::size_t & j, std::string & str) {

    assert(i >= 0 && j >= 0 && i <= j && i < _g.get_size_text() && j < _g.get_size_text()); // border assert

    // FIND THE NODE WHERE i OCCURS
    const auto& Tg = _g.get_parser_tree();
    long long int p = i;

    std::stack<dfuds::dfuds_tree::dfuds_long> s_path;
    dfuds::dfuds_tree::dfuds_long root =Tg.root();

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

        dfuds::dfuds_tree::dfuds_long  l = Tg.find_child(node,ls,hs,[&p,this](const dfuds::dfuds_tree::dfuds_long &child)->bool{
            size_t pos_m = _g.offsetText(child);
            return p < pos_m;
        });


        next_node = Tg.child(node,l);
        return true;
    });

    long long int off = j-i+1;
    str.resize(off);
    size_t pos = 0;
    expand_prefix(_g[Tg.pre_order(s_path.top())],str,(size_t)off,pos);

    while(!s_path.empty() && pos < off ){

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

}

unsigned long SelfGrammarIndex::size_in_bytes() const {
    return _g.size_in_bytes() + grid.size_in_bytes();
}

int SelfGrammarIndex::cmp_prefix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) {

    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < *itera) return 1;
        if(a_th > *itera) return  -1;
        ++itera;
        ///if(itera == end) return 0;
        return 0;
    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& dfuds_rep = tree.get_tree();
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
    size_t  node_xi_left_path = dfuds_rep[pre_tlmp_xi];

    /*
     * Finding path to the root of the left most path trie
     * */
    std::deque<size_t > S;
    S.emplace_front(node_xi_left_path);
    size_t current = node_xi_left_path;


    while(dfuds_rep.root() && dfuds_rep.parent(current) != dfuds_rep.root()){
        current = dfuds_rep.parent(current); // DFUDS DO NOT SUPPORT DEPTH OP
        S.emplace_front(current);
    }

    /*
     *  root's child ancestor
     * */
    size_t  X_a = seq[dfuds_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < *itera) return 1;
        if(a_th > *itera) return -1;
        ++itera;
        if(itera == end) return 0;

        S.pop_front();
    }


    while(!S.empty())
    {

        size_t  top = S.front();
        size_t  preoreder_trie = dfuds_rep.pre_order(top);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        size_t  begin = (top == current)?1:2;

        for (size_t  i = begin ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            auto r = cmp_prefix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end)
                return r;
        }

        S.pop_front();

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
    size_t pos = 0;

    auto r = cmp_prefix(X,iterator1,iterator2);

    if(r == 0)
    {
        auto node = parser_tree[sfx_preorder];
        auto parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;

        while(r == 0 && iterator1!=iterator2 && child_r <= n ){
            //std::string s1;
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

int SelfGrammarIndex::cmp_suffix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) {

    assert(X_i > 0 && X_i < _g.n_rules());


    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < *(itera)) return 1;
        if(a_th > *(itera)) return  -1;
        --itera;

        return 0;
        ////if(itera == end) return 0;

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

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < *(itera)) return 1;
        if(a_th > *(itera)) return  -1;
        --itera;
        if(itera == end-1) return 0;

        S.pop_front();
    }


    while(!S.empty()){


        size_t  top = S.front();
        size_t  preoreder_trie = dfuds_rep.pre_order(top);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        size_t  begin = (top == current)?n_ch:n_ch-1;

        for (size_t  i = begin ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            auto r = cmp_suffix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end-1 )
                return r;
        }

        S.pop_front();
    }

    return 0;
}

bool SelfGrammarIndex::bp_expand_prefix(const compressed_grammar::g_long &X_i,std::string & s, const size_t & l,size_t & pos) const
{

    assert(X_i > 0 && X_i < _g.n_rules());

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


    return false;
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


    return false;

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

void SelfGrammarIndex::locate_ch(const char & ch, sdsl::bit_vector & occ) {

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
