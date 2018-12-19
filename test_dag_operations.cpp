#include <iostream>
#include <sstream>
#include "dag.h"
#include "citation_graph.h"
#include "publication.h"
#include <cassert>
using namespace std;


int main() {
    vector<pair<int, int>> connections = Dag<int>::read_stdin();
    Dag<int> d;
    for (const auto &p : connections) {
        d.addEdge(p.first, p.second);
    }

    CitationGraph<Publication<int, 0>> graph(d.getRoot());
    for (const auto &p : connections) {
        if (!graph.exists(p.second))
            graph.create(p.second, p.first);
        else
            graph.add_citation(p.second, p.first);
    }

    std::ostringstream dag_serialize;
    std::ostringstream graph_serialize;
    dag_serialize << d;
    graph_serialize << graph;
    assert(dag_serialize.str() == graph_serialize.str());
}







