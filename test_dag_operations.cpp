#include <iostream>
#include <sstream>
#include "dag.h"
#include "publication.h"
#include <algorithm>
using namespace std;

using IDag = Dag<int>;
using ICitationGraph =  CitationGraph<Publication<PublicationId>>;

void add_with_parents(int node, IDag &d, ICitationGraph &g) {
    vector<PublicationId> parents;
    for (int p: d.parents[node]) {
        if (!g.exists(p)) {
            add_with_parents(p, d, g);
        }
        parents.emplace_back(p);
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
            assert(graph.exists(p.first));
            assert(graph.exists(p.second));
        }
        IDag::assert_same(d, graph);
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
        ICitationGraph graph(d.get_root());
        for (const auto &p : raw_input) {
            if (!graph.exists(p.second))
                graph.create(p.second, p.first);
            else
                graph.add_citation(p.second, p.first);
        }
        int root = d.get_root();
        vector<int> keys;
        for(auto const &[k, v]: d.children){
            keys.push_back(k);
        }
        random_shuffle(keys.begin(), keys.end());
        for (auto const &key : keys) {
            if (key == root || d.children.count(key) == 0) continue;
            cout << "removing " << key << endl;
            d.remove_vertex(key);
            graph.remove(key);
            IDag::assert_same(d, graph);
        }

        //d.remove_vertex(2);
        std::cout << d;
    }
}







