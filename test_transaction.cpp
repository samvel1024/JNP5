#include "citation_graph.h"
#include <iostream>
#include <cassert>
#include <exception>
using namespace std;

/**
 * For all x belonging to to_be_added adds x to s only if (x-1) belongs to s
 * Otherwise throws exception
 * @param s
 * @param to_be_added
 */
void try_add(std::map<int, bool> &s, std::vector<int> &to_be_added) {

    Transaction<std::map<int, bool>> transaction;
    for (auto &i: to_be_added) {
        if (s.find(i-1) == s.end()){
            throw std::exception();
        }else {
            auto it = s.begin();
            auto added = s.insert(it, make_pair(i, false));
            transaction.add(s, added);
        }
    }

    transaction.commit();

}


int main() {
    
    map<int, bool> m{{1, true}, {3, false}, {5, true}, {7, false}};
    try{
        vector<int> to_be_added = {2, 4, 10};
        try_add(m, to_be_added);
        exit(1);
    }catch(std::exception &e){
        assert(m.size() == 4);
    }

    try{
        vector<int> to_be_added = {2, 4, 6, 8};
        try_add(m, to_be_added);
        assert(m.size() == 8);
    }catch (std::exception &e){
        exit(1);
    }

    return 0;
}