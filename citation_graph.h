#ifndef CITATIONGRAPH_H
#define CITATIONGRAPH_H

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <map>
#include <ostream>
#include <iostream>
#include <ostream>
#include <cassert>
#include <sstream>

#ifdef DEBUG
#define LOG(x) x<<std::endl;
#else
#define LOG(x)
#endif

class PublicationAlreadyCreated : public std::exception {
    char const *what() const noexcept override { return "PublicationAlreadyCreated"; }
};

class PublicationNotFound : public std::exception {
    char const *what() const noexcept override { return "PublicationNotFound"; }
};

class TriedToRemoveRoot : public std::exception {
    char const *what() const noexcept override { return "TriedToRemoveRoot"; }
};

template<typename Container>
class Transaction {
private:
    enum Operation {
        REMOVAL, ADDITION
    };
    std::vector<std::tuple<Container &, typename Container::iterator, Operation>> rollback_log{};
    bool failed;

public:
    explicit Transaction() : failed(true) {};

    virtual ~Transaction() {
        for (auto &i : rollback_log) {
            auto &[container, iter, type] = i;
            if ((type == ADDITION && failed) || (type == REMOVAL && !failed)) {
                container.erase(iter);
            }
        }
    }

    void commit() { failed = false; }

    void record_addition(Container &c, typename Container::iterator iter) {
        rollback_log.emplace_back(c, iter, ADDITION);
    }

    void record_removal(Container &c, typename Container::iterator iter) {
        rollback_log.emplace_back(c, iter, REMOVAL);
    }
};


template<typename Publication>
class CitationGraph {
private:
    class Node;

    template<typename T>
    struct WeakComparator;
    template<typename T>
    struct PtrComparator;

    using NodeId = typename Publication::id_type;
    using ParentSet = std::set<Node *>;
    using ChildSet = std::set<std::shared_ptr<Node>, PtrComparator<std::shared_ptr<Node>>>;
    using NodeLookupMap = std::map<NodeId, std::weak_ptr<Node>>;


    template<typename T>
    struct PtrComparator {
        bool operator()(const T &lhs, const T &rhs) const {
            return (*lhs) < (*rhs);
        }
    };


    class Node {
    private:
        Publication value;
        ParentSet parents;
        ChildSet children;
        typename NodeLookupMap::iterator iter;
        NodeLookupMap &map;
        NodeId id;

    public:
        explicit Node(NodeId id, NodeLookupMap &m) :
            value(id), parents(), children(), id(id), map(m) {}


        virtual ~Node() {
            LOG(std::cout << "Destruction of node with id: " << id << std::endl);
            for (auto &c: children) {
                c->parents.erase(this);
            }
            children.clear();
            map.erase(iter);
        }

        void set_lookup_iterator(typename NodeLookupMap::iterator iter) {
            this->iter = iter;
        }

        typename ParentSet::iterator add_parent(Node *ptr) {
            return parents.emplace(ptr).first;
        }

        typename ChildSet::iterator add_child(const std::shared_ptr<Node> ptr) {
            return children.insert(ptr).first;
        }

        const Publication &get_publication() const noexcept { return value; }

        ParentSet &get_parent_set() noexcept { return parents; }

        ChildSet &get_child_set() noexcept { return children; }


        friend std::ostream &operator<<(std::ostream &os, const Node &node) {
            os << "Node {value= " << &node.value << "}";
            return os;
        }

        bool operator<(const Node &rhs) const {
            return this->value.get_id() < rhs.value.get_id();
        }
    };

    typename NodeLookupMap::const_iterator find_or_throw(NodeId const &id) const {
        auto node = publication_ids.find(id);
        if (node == publication_ids.end()) {
            throw PublicationNotFound();
        }
        return node;
    }

    NodeLookupMap publication_ids;
    std::shared_ptr<Node> source; //TODO does this have to be shared_ptr??
    NodeId source_id;


    //TODO replace with dereferencing struct template, integrate with comparators too

    std::vector<NodeId> to_vector(ChildSet &s) const {
        std::vector<NodeId> vec;
        vec.reserve(s.size());
        for (auto &elem: s) {
            auto &drf = *elem;
            vec.push_back(drf.get_publication().get_id());
        }
        return vec;
    }

    std::vector<NodeId> to_vector_parent(ParentSet &s) const {
        std::vector<NodeId> vec;
        vec.reserve(s.size());
        for (auto *ptr: s) {
            vec.push_back(ptr->get_publication().get_id());
        }
        return vec;
    }

public:

    explicit CitationGraph(NodeId const &stem_id) : source_id(stem_id) {
        std::shared_ptr<Node> root = std::make_shared<Node>(stem_id, publication_ids);
        auto iter = publication_ids.insert(publication_ids.begin(), std::make_pair(stem_id, root));
        root->set_lookup_iterator(iter);
        this->source = root;
    }

    CitationGraph(CitationGraph<Publication> &&other) noexcept
        : publication_ids(), source(nullptr), source_id(other.stem_id) {
        *this = std::move(other);
    }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept {
        std::swap(this->publication_ids, std::move(other.publication_ids));
        std::swap(this->source, std::move(other.source));
        std::swap(this->source_id, std::move(other.source_id));
    }

    NodeId get_root_id() const {
        return this->source_id;
    }

    std::vector<NodeId> get_children(NodeId const &id) const {
        ChildSet &c = find_or_throw(id)->second.lock()->get_child_set();
        return to_vector(c);
    }

    std::vector<NodeId> get_parents(NodeId const &id) const {
        ParentSet &a = find_or_throw(id)->second.lock()->get_parent_set();
        return to_vector_parent(a);
    }

    bool exists(NodeId const &id) const {
        auto iter = publication_ids.find(id);
        return iter != publication_ids.end() && !(*iter).second.expired();
    }

    const Publication &operator[](NodeId const &id) const {
        return find_or_throw(id)->second.lock()->get_publication();
    }

    void create(NodeId const &id, NodeId const &parent_id) {
        create(id, std::vector<NodeId>(1, parent_id));
    }

    void create(NodeId const &id, std::vector<NodeId> const &parent_ids) {
        if (publication_ids.find(id) != publication_ids.end()) {
            throw PublicationAlreadyCreated();
        }

        if (parent_ids.empty()) {
            throw PublicationNotFound();
        }

        Transaction<ChildSet> c_trans;
        Transaction<ParentSet> p_trans;
        Transaction<NodeLookupMap> nl_trans;

        std::shared_ptr<Node> child = std::make_shared<Node>(id, publication_ids);
        auto lookup_iterator = publication_ids.insert(
            publication_ids.begin(),
            std::make_pair(id, child));
        nl_trans.record_addition(publication_ids, lookup_iterator);
        for (NodeId parent_id : parent_ids) {
            if (parent_id == id) {
                throw PublicationNotFound();
            }
            auto parent_iter = publication_ids.find(parent_id);
            if (parent_iter == publication_ids.end()) {
                throw PublicationNotFound();
            }
            std::shared_ptr<Node> parent = (*parent_iter).second.lock();
            auto c_iter = parent->add_child(child);
            auto p_iter = child->add_parent(parent.get());
            c_trans.record_addition(parent->get_child_set(), c_iter);
            p_trans.record_addition(child->get_parent_set(), p_iter);
        }

        child->set_lookup_iterator(lookup_iterator);
        nl_trans.commit();
        p_trans.commit();
        c_trans.commit();

    }


    void add_citation(NodeId const &child_id, NodeId const &parent_id) {
        typename NodeLookupMap::iterator child_iter = publication_ids.find(child_id);
        typename NodeLookupMap::iterator parent_iter = publication_ids.find(parent_id);
        if (child_iter == publication_ids.end() ||
            parent_iter == publication_ids.end() || child_id == parent_id) {
            throw PublicationNotFound();
        }

        Transaction<ChildSet> c_trans;
        Transaction<ParentSet> p_trans;

        std::shared_ptr<Node> parent = parent_iter->second.lock();
        std::shared_ptr<Node> child = child_iter->second.lock();
        p_trans.record_addition(child->get_parent_set(), child->add_parent(parent.get()));
        c_trans.record_addition(parent->get_child_set(), parent->add_child(child));

        c_trans.commit();
        p_trans.commit();
    }

    void remove(NodeId const &base_remove_id) {
        auto map_iter = publication_ids.find(base_remove_id);
        if (map_iter == publication_ids.end()) {
            throw PublicationNotFound();
        }
        if (base_remove_id == source_id) {
            throw TriedToRemoveRoot();
        }

        Transaction<ChildSet> t;

        auto node = (*map_iter).second.lock();
        for (auto &p : node->get_parent_set()) {
            auto i = p->get_child_set().find(node);
            assert(i != p->get_child_set().end());
            t.record_removal(p->get_child_set(), i);
        }

        t.commit();
    }

    friend std::ostream &operator<<(std::ostream &os, const CitationGraph &cg) {
        for (auto &pair : cg.publication_ids) {
            std::shared_ptr<Node> node = (pair.second.lock());
            os << "Children of " << pair.first << ": ";
            for (auto const &c : node->get_child_set()) {
                os << c->get_publication().get_id() << " ";
            }
            os << std::endl;
            os << "Parents of " << pair.first << ": ";
            std::set<NodeId> s;
            for (auto const &p: node->get_parent_set()) {
                s.insert(p->get_publication().get_id());
            }
            for (auto const &p: s) {
                os << p << " ";
            }
            os << std::endl;
        }
        os << std::endl;
        return os;

    }

    std::string to_string(){
       std::ostringstream stream;
       stream << *this;
       return stream.str();
    }
};

#endif
