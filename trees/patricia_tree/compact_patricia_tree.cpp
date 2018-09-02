//
// Created by inspironXV on 8/17/2018.
//

#include "compact_patricia_tree.h"

using namespace m_patricia;

m_patricia::compact_patricia_tree::compact_patricia_tree() {

}

m_patricia::compact_patricia_tree::~compact_patricia_tree() {

}

void m_patricia::compact_patricia_tree::build(const m_patricia::compact_patricia_tree::T &tree) {


    size_t n = tree.num_total_nodes();
    size_t m_n = tree.num_leaves_rep();

    sdsl::int_vector<> _jumps((n+m_n)+1,0);
    sdsl::int_vector<> _s((n+m_n)+1,0);
    _s[0] = '_';
    _s[1] = '_';
    sdsl::bit_vector b_temp((n+m_n)*2-1,1);
    size_t   pre = 1,pre_s = 2, off = 0;

    tree.dfs(tree.root(),[&_jumps,&_s,&b_temp,&pre_s,&pre, &off](const m_patricia::node *n, const char &a, const ulong &j)->bool
    {

        size_t ch = n->childs.size();
        size_t  m = n->ids.size();

        /*
         * if there is a duplicated prefix
         *
         * */
        if( m > 1 && ch == 0){
            b_temp[off+m] = false;
            off+=m+1;
            //////_s[pre_s] = a;
            _jumps[pre] = j;
            ++pre;
            //////++pre_s;

            for (size_t  i = 0; i < n->ids.size() ; ++i) {
                b_temp[off] = false;
                _s[pre_s] = 1;//(int)'#';
                _jumps[pre] = 1;
                ++pre;
                ++off;
                ++pre_s;
            }

            return false;
        }

        if(ch == 0)
        {
            b_temp[off] = false;
            off++;
            _jumps[pre] = j;
            ++pre;

            return false;
        }


        b_temp[off+ch] = false;
        off += ch+1;

        for (auto &&item : n->childs) {
            _s[pre_s] = item.first;
            ++pre_s;
        }

        _jumps[pre] = j;
        ++pre;
        return ch != 0;

    },'_',0);

    m_tree.build(b_temp);
    jumps = seq_jmp(_jumps);
    sdsl::util::bit_compress(jumps);
    seq = seq_alf(_s);
    sdsl::util::bit_compress(seq);

}


void m_patricia::compact_patricia_tree::build(const m_patricia::patricia_tree<m_patricia::rev_string_pairs> &tree) {


    size_t n = tree.num_total_nodes();
    size_t m_n = tree.num_leaves_rep();

    sdsl::int_vector<> _jumps((n+m_n)+1,0);
    sdsl::int_vector<> _s((n+m_n)+1,0);
    _s[0] = '_';
    _s[1] = '_';
    sdsl::bit_vector b_temp((n+m_n)*2-1,1);
    size_t   pre = 1,pre_s = 2, off = 0;

    tree.dfs(tree.root(),[&_jumps,&_s,&b_temp,&pre_s,&pre, &off](const m_patricia::node *n, const char &a, const ulong &j)->bool
    {

        size_t ch = n->childs.size();
        size_t  m = n->ids.size();

        /*
         * if there is a duplicated prefix
         *
         * */
        if( m > 1 && ch == 0){
            b_temp[off+m] = false;
            off+=m+1;
           //// _s[pre_s] = a;
            _jumps[pre] = j;
            ++pre;
            ////++pre_s;

            for (size_t  i = 0; i < n->ids.size() ; ++i) {
                b_temp[off] = false;
                _s[pre_s] = 1;
                _jumps[pre] = 1;
                ++pre;
                ++off;
                ++pre_s;
            }

            return false;
        }

        if(ch == 0)
        {
            b_temp[off] = false;
            off++;
            _jumps[pre] = j;
            ++pre;
            return false;
        }


        b_temp[off+ch] = false;
        off += ch+1;

        for (auto &&item : n->childs) {
            _s[pre_s] = item.first;
            ++pre_s;
        }

        _jumps[pre] = j;
        ++pre;
        return ch != 0;

    },'_',0);

    m_tree.build(b_temp);
    jumps = seq_jmp(_jumps);
    sdsl::util::bit_compress(jumps);
    seq = seq_alf(_s);
    sdsl::util::bit_compress(seq);

}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_match(const m_patricia::compact_patricia_tree::K &str) const {
    ulong node = m_tree.root();
    compact_patricia_tree::ulong p = 0;
    path(node,str,p);
    return node;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_match(const m_patricia::compact_patricia_tree::revK &str) const {
    ulong node = m_tree.root();
    compact_patricia_tree::ulong p = 0;
    path(node,str,p);
    return node;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_locus(const m_patricia::compact_patricia_tree::K & str, const compact_patricia_tree::ulong & limit)const {
    compact_patricia_tree::ulong node = m_tree.root();
    compact_patricia_tree::ulong p = 0;
    path(node,str,p,limit);
    return node;
}

void m_patricia::compact_patricia_tree::save(std::fstream & f) const {

    sdsl::serialize(seq,f);
    sdsl::serialize(jumps,f);
    m_tree.save(f);

}

bool m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::K &str,
                                             compact_patricia_tree::ulong & p) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
    {
        return true ;
    }

    if(p >= str.size())
        return true;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);
    auto iii = *t;
    if(t > seq.begin()+r)
        return true;
    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return true;

    size_t child = m_tree.child(node, ppp );

    size_t ii = m_tree.pre_order(child);

    p += jumps[ii];

    node = child;

    return path(node,str,p);

}

bool m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::revK &str,
                                             compact_patricia_tree::ulong & p) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
    {
        return true ;
    }

    if(p >= str.size())
        return true;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);

    char iii = *t;

    if(t > seq.begin()+r)
        return true;

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return true;

    size_t child = m_tree.child(node, ppp );


    size_t ii = m_tree.pre_order(child);

    p += jumps[ii];

    node = child;

    return path(node,str,p);

}

bool m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::K & str, compact_patricia_tree::ulong & p, const compact_patricia_tree::ulong &limit) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0){
        return true ;
    }

    if(p >= str.size())
        return true;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l,seq.begin()+r,(unsigned char)str[p]);
    auto iii = *t;
    if(t == seq.begin()+r)
        return true;

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);
    size_t child = m_tree.child(node, ppp );

    size_t ii = m_tree.pre_order(child);

    p += jumps[ii];

    if( p >= limit )
        return true;

    node = child;

    return path(node,str,p);
}

void m_patricia::compact_patricia_tree::load(std::fstream &f) {

    sdsl::load(seq,f);
    sdsl::load(jumps,f);
    m_tree.load(f);

}

const m_patricia::compact_patricia_tree::tree &m_patricia::compact_patricia_tree::get_tree() const {
    return m_tree;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::size_in_bytes() const {

    return m_tree.size_in_bytes() + sdsl::size_in_bytes(jumps) + sdsl::size_in_bytes(seq);

}

void m_patricia::compact_patricia_tree::print_size_in_bytes() const {

    std::cout<<"\t tree size "<<m_tree.size_in_bytes()<<std::endl;
    std::cout<<"\t sequence size "<<sdsl::size_in_bytes(seq)<<std::endl;
    std::cout<<"\t tree size "<<sdsl::size_in_bytes(jumps)<<std::endl;

}

compact_patricia_tree &compact_patricia_tree::operator=(const m_patricia::compact_patricia_tree& T) {
    m_tree = T.m_tree;
    seq = T.seq;
    jumps = T.jumps;
    return (*this);
}

const compact_patricia_tree::seq_alf &compact_patricia_tree::get_seq() const {
    return seq;
}

const compact_patricia_tree::seq_jmp &compact_patricia_tree::get_jumps() const {
    return jumps;
}
