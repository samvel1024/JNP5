#include <iostream>
#include <sstream>
#include "dag.h"
#include "publication.h"

using namespace std;

using IDag = Dag<int>;
using ICitationGraph =  CitationGraph<Publication<int, 0>>;


void add_with_parents(int node, IDag &d, ICitationGraph &g) {
    vector<int> parents;
    for (int p: d.parents[node]) {
        if (!g.exists(p)) {
            add_with_parents(p, d, g);
        }
        parents.push_back(p);
    }
    g.create(node, parents);
}


void add(int node, IDag &d, ICitationGraph &g) {
    if (!g.exists(node)) {
        add_with_parents(node, d, g);
    }
    for (int c: d.children[node]) {
        add(c, d, g);
    }
}


int main() {


    vector<pair<int, int>> raw_input = IDag::read_raw();
    { //Test addition one by one
        IDag d = IDag::from_vector(raw_input);

        ICitationGraph graph(d.get_root());
        for (const auto &p : raw_input) {
            if (!graph.exists(p.second))
                graph.create(p.second, p.first);
            else
                graph.add_citation(p.second, p.first);
        }
        IDag::assert_same(d, graph);
        std::cout << d;
    }
    {//Test batch addition (VERY SLOW)
        IDag d = IDag::from_vector(raw_input);
        int root = d.get_root();
        ICitationGraph g(root);
        add(root, d, g);
        IDag::assert_same(d, g);
    }
    {//Test removal

        IDag d = IDag::from_vector(raw_input);
        //ICitationGraph graph(d.get_root());
/*        for (const auto &p : raw_input) {
            if (!graph.exists(p.second))
                graph.create(p.second, p.first);
            else
                graph.add_citation(p.second, p.first);
        }
        int root = d.get_root();
        for (auto const&[key, val] : d.children) {
            if (key == root) continue;
            d.remove_vertex(key);
            graph.remove(key);
            IDag::assert_same(d, graph);
        }*/

        //d.remove_vertex(2);
        std::cout << d;
    }
}







