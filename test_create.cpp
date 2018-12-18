#include <iostream>
#include "CitationGraph.h"
#include "Publication.h"

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
            std::cout << 1;
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

}