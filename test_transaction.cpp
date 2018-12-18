#include "CitationGraph.h"
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
void try_add(std::set<int> &s, std::vector<int> &to_be_added) {

    Transaction<std::set<int>, std::set<int>::iterator> transaction(s);
    for (auto &i: to_be_added) {
        if (s.find(i-1) == s.end()){
            throw std::exception();
        }else {
            auto added = s.emplace(i);
            transaction.add(added.first);
        }
    }

    transaction.commit();

}


int main() {
    
    set<int> set{1, 3, 5, 7};
    try{
        vector<int> to_be_added = {2, 4, 10};
        try_add(set, to_be_added);
        exit(1);
    }catch(std::exception &e){
        assert(set.size() == 4);
    }

    try{
        vector<int> to_be_added = {2, 4, 6, 8};
        try_add(set, to_be_added);
        assert(set.size() == 8);
    }catch (std::exception &e){
        exit(1);
    }

    return 0;
}