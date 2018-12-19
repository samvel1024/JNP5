#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;

template<typename T>
class Dag {
public:
    map<T, set<T>> children;
    map<T, set<T>> parents;
    T root;

    Dag(T root) : root(root) {
        children.emplace(root, set<T>());
        parents.emplace(root, set<T>());
    }

    void addVertex(const T &new_node, const T &parent) {
        parents[new_node] = set<T>{parent};
        children[new_node] = set<T>();
        children[parent].emplace(new_node);
    }

    void addVertices(const T &new_node, const vector<T> &node_parents){
        parents[new_node] = set<T>();
        children[new_node] = set<T>();
        for(auto &p :node_parents){
            parents[new_node].emplace(p);
            children[p].emplace(new_node);
        }
    }

    void addEdge(const T &from, const T &to){
        children[from].emplace(to);
        parents[to].emplace(from);
    }

    void remove_vertex(const T &v){
        for(auto &c: children[v]){
            parents[c].erase(v);
        }

        for(auto &p:parents[v]){
            children[p].erase(v);
        }

        parents.erase(v);
        children.erase(v);
    }


    friend ostream &operator<<(ostream &os, Dag &dag) {
        for( auto  &pair : dag.children ){
            cout << "Children of " << pair.first << ": ";
            for(auto const &c : pair.second){
                cout << c << " ";
            }
            cout << endl;
            cout << "Parents of " << pair.first << ": ";
            set<T> &parents = dag.parents[pair.first];
            for(auto const &p: parents){
                cout << p << " ";
            }
            cout << endl;
        }
        cout << endl;
        return os;
    }

};


int main() {
    auto d = Dag<int>(1);
    d.addVertex(2, 1);
    d.addVertex(3, 1);
    d.addVertices(4, vector<int>{2, 3});
    d.remove_vertex(2);
    cout << d;
}