#ifndef JNP5_TEST_DAG_H
#define JNP5_TEST_DAG_H

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

    Dag() = default;

    void addVertex(const T &new_node, const T &parent) {
        parents[new_node] = set<T>{parent};
        children[new_node] = set<T>();
        children[parent].emplace(new_node);
    }

    void addVertices(const T &new_node, const vector<T> &node_parents) {
        parents[new_node] = set<T>();
        children[new_node] = set<T>();
        for (auto &p :node_parents) {
            parents[new_node].emplace(p);
            children[p].emplace(new_node);
        }
    }


    T getRoot(){
        for(auto &p : parents){
            if (p.second.empty())
                return p.first;
        }
        throw std::exception();
    }

    void addIfAbsent(map<T, set<T>> &m, const T &node) {
        if (m.find(node) == m.end()) {
            m[node] = set<T>();
        }
    }

    void addIfAbsent(const T &node) {
        addIfAbsent(children, node);
        addIfAbsent(parents, node);
    }

    void addEdge(const T &from, const T &to) {
        addIfAbsent(from);
        addIfAbsent(to);
        children[from].emplace(to);
        parents[to].emplace(from);
    }

    void remove_vertex(const T &v) {
        for (auto &c: children[v]) {
            parents[c].erase(v);
        }

        for (auto &p:parents[v]) {
            children[p].erase(v);
        }

        parents.erase(v);
        children.erase(v);
    }


    static vector<pair<int, int>> read_stdin() {
        int a, b;
        vector<pair<int, int>> v;
        while (cin >> a) {
            cin >> b;
            v.emplace_back(a, b);
        }
        return v;
    }

    friend ostream &operator<<(ostream &os, Dag &dag) {
        for (auto &pair : dag.children) {
            cout << "Children of " << pair.first << ": ";
            for (auto const &c : pair.second) {
                cout << c << " ";
            }
            cout << endl;
            cout << "Parents of " << pair.first << ": ";
            set<T> &parents = dag.parents[pair.first];
            for (auto const &p: parents) {
                cout << p << " ";
            }
            cout << endl;
        }
        cout << endl;
        return os;
    }

};


#endif //JNP5_TEST_DAG_H
