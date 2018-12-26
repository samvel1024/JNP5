#include <iostream>
#include <sstream>
#include "dag.h"
#include "publication.h"

using namespace std;

using IDag = Dag<int>;
using ICitationGraph =  CitationGraph<Publication<PublicationId>>;


int main() {

    vector<pair<int, int>> raw_input = IDag::read_raw();
    {//Test removal

        IDag d = IDag::from_vector(raw_input);
        ICitationGraph graph(d.get_root());
        for (const auto &p : raw_input) {
            if (!graph.exists(p.second))
                graph.create(p.second, p.first);
            else
                graph.add_citation(p.second, p.first);
        }
        int root = d.get_root();
        vector<int> keys;
        for (auto const &[k, v]: d.children) {
            keys.push_back(k);
        }
        random_shuffle(keys.begin(), keys.end());
        for (auto const &key : keys) {
            if (key == root || d.children.count(key) == 0) continue;
            PublicationId::set_exception_prob(0);
            string before = graph.to_string();
            try {
                PublicationId::set_exception_prob(10);
                cout << "Removing key=" << key << endl;
                graph.remove(key);
                PublicationId::set_exception_prob(0);
                d.remove_vertex(key);
                IDag::assert_same(d, graph);
            }
            catch (ComparisonException &e) {
                PublicationId::set_exception_prob(0);
                cout << "Catching exception when removing key="<<key << endl;
                string after = graph.to_string();
                assert(before == after);
            }
        }

        std::cout << d;
    }
}







