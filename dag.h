#ifndef JNP5_TEST_DAG_H
#define JNP5_TEST_DAG_H

#include <iostream>
#include <map>
#include <set>
#include "citation_graph.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <cstdio>
using namespace std;

#ifdef INPUTFILE
void *___init_(){
    return freopen(INPUTFILE, "r", stdin);
}
void *__ptr = ___init_();
#endif

template<typename T>
class Dag {
public:
    map<T, set<T>> children;
    map<T, set<T>> parents;

    Dag() = default;

    void add_vertex(const T &new_node, const T &parent) {
        parents[new_node] = set<T>{parent};
        children[new_node] = set<T>();
        children[parent].emplace(new_node);
    }

    void add_vertices(const T &new_node, const vector<T> &node_parents) {
        parents[new_node] = set<T>();
        children[new_node] = set<T>();
        for (auto &p :node_parents) {
            parents[new_node].emplace(p);
            children[p].emplace(new_node);
        }
    }


    T get_root(){
        for(auto &p : parents){
            if (p.second.empty())
                return p.first;
        }
        throw std::exception();
    }

    void addIfAbsent(map<T, set<T>> &m, const T &node) {
        if (m.find(node) == m.end()) {
            m[node] = set<T>();
        }
    }

    void add_if_absent(const T &node) {
        addIfAbsent(children, node);
        addIfAbsent(parents, node);
    }

    void add_edge(const T &from, const T &to) {
        add_if_absent(from);
        add_if_absent(to);
        children[from].emplace(to);
        parents[to].emplace(from);
    }

    bool is_orphan(const T&v){
        return parents[v].empty();
    }

    void remove_orphan(const T&v){
        assert(is_orphan(v));
        for(auto &c: children[v]){
            parents[c].erase(v);
            if (is_orphan(c)){
                remove_orphan((c));
            }
        }
        parents.erase(v);
        children.erase(v);
    }


    void remove_vertex(const T &v) {
        for (auto &p:parents[v]) {
            children[p].erase(v);
        }
        parents[v].clear();
        remove_orphan(v);
    }

    static vector<pair<int, int>> read_raw() {
        int a, b;
        vector<pair<int, int>> v;
        while (cin >> a) {
            cin >> b;
            v.emplace_back(a, b);
        }
        return v;
    }

    static Dag<int> from_vector(const vector<pair<int, int>> &connections) {
        Dag<int> d;
        for (const auto &p : connections) {
            d.add_edge(p.first, p.second);
        }
        return d;
    }


    template <typename P>
    static void assert_same(Dag<int> &d, CitationGraph<P> &g){
        std::ostringstream dag_serialize;
        std::ostringstream graph_serialize;
        dag_serialize << d;
        graph_serialize << g;
        assert(dag_serialize.str() == graph_serialize.str());
    }
    template <typename P>
    static string to_string(P &g){
        std::ostringstream str;
        str << g;
        return str.str();
    }

    friend ostream &operator<<(ostream &os, Dag &dag) {
        for (auto &pair : dag.children) {
            os << "Children of " << pair.first << ": ";
            for (auto const &c : pair.second) {
                os << c << " ";
            }
            os << endl;
            os << "Parents of " << pair.first << ": ";
            set<T> &parents = dag.parents[pair.first];
            for (auto const &p: parents) {
                os << p << " ";
            }
            os << endl;
        }
        os << endl;
        return os;
    }

};


#endif //JNP5_TEST_DAG_H
