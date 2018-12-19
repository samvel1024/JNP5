#include <iostream>
#include "dag.h"
#include "citation_graph.h"
#include "publication.h"
using namespace std;



int main(){
    vector<pair<int, int>>  connections = Dag<int>::read_stdin();
    Dag<int> d;
    for(const auto &p : connections){
        d.addEdge(p.first, p.second);
    }
    cout << d;

    CitationGraph<Publication<int, 0>>  graph(d.getRoot());
    for(const auto&p : connections){
        graph.create(p.second, p.first);
    }

    cout << endl;
    cout << graph;

}







