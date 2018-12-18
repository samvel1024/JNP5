#include <iostream>
#include "CitationGraph.h"
#include "Publication.h"


int main() {
    CitationGraph<Publication<int>> graph(23);
    return 0;
}