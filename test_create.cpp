#include <iostream>
#include "CitationGraph.h"

template <typename T>
class Publication {
private:
    T id;

public:
    typedef T id_type;

    Publication(T arg) : id(arg) {}

    T get_id() const {
        return id;
    };
};


int main() {
    CitationGraph<Publication<int>> graph(1);
    graph.create(2, 1);
    return 0;
}