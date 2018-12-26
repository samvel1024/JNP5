#include <iostream>
#include "citation_graph.h"
#include "publication.h"

using namespace std;

int main() {
    {
        CitationGraph<Publication<int>> graph(1);
        graph.create(2, 1);
        graph.create(3, 1);
        try {
            const std::vector<int> v{2, 3, 5};
            graph.create(4, v);
            exit(1);
        }catch (PublicationNotFound &e){
            //TODO assert graph not changed
        }
    }
    {
        CitationGraph<Publication<char>> graph('a');
        graph.create('b', 'a');
        graph.create('c', 'a');
        try {
            const std::vector<char> v{'c', 'b'};
            graph.create('d', v);
        }catch (std::exception &e){
            exit(1);
        }
    }
    {
        CitationGraph<Publication<string>> graph("a");
        graph.create("b", "a");
        graph.create("c", "a");
        try {
            const std::vector<string> v{"c", "b"};
            graph.create("d", v);
        }catch (std::exception &e){
            exit(1);
        }
        graph["a"];
    }

}