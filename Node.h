
#include <vector>
#include <string>
#include <set>
#include <memory>
#include <map>
#include <ostream>


class Node {
    using ParentSet = std::set<std::weak_ptr<Node>>;
    using ChildSet = std::set<std::shared_ptr<Node>>;
private:
    Publication value;
    ParentSet parents;
    ChildSet children;

public:
    Node(id_type id) : value(id), parents(), children() {}


    ParentSet::iterator add_parent(const std::shared_ptr<Node> &ptr) {
        return parents.emplace(std::weak_ptr<Node>(ptr)).first;
    }

    void remove_parent(typename std::set<std::shared_ptr<Node>>::iterator it) {
        parents.erase(it);
    }

    ChildSet::iterator add_child(const std::shared_ptr<Node> &ptr) {
        return children.emplace(std::shared_ptr<Node>(ptr)).first;
    }

    void remove_child(typename std::set<std::shared_ptr<Node>>::iterator it) {
        children.erase(it);
    }

    const Publication &get_publication() const {
        return value;
    }


    ParentSet &get_parents() {
        return parents;
    }

    ChildSet &get_children() {
        return children;
    }

    std::vector<id_type> get_children() const {
        std::vector<id_type> vec;
        for (auto child : children) {
            vec.emplace(child->get_publication().get_id());
        }
        return vec;
    }

    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "Node {value= " << &node.value << "}";
        return os;
    }

    virtual ~Node() {
        LOG(std::cout << "Destruction " << *this);
    }
};
