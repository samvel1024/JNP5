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
    std::vector<std::pair<Container &, typename Container::iterator>>
        to_be_removed{};
    bool failed;

public:
    explicit Transaction() : failed(true) {};

    virtual ~Transaction() {
        std::cout << to_be_removed.size() << std::endl;
        if (this->failed) {
            for (auto &i : to_be_removed) {
                std::cout << "tick" << std::endl;
                i.first.erase(i.second);
            }
        }
        std::cout << "done" << std::endl;
    }

    void commit() { failed = !failed; }

    void add(Container &c, typename Container::iterator iter) {
        to_be_removed.emplace_back(c, iter);
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
    using ParentSet = std::set<std::weak_ptr<Node>, WeakComparator<Node>>;
    using ChildSet = std::set<std::shared_ptr<Node>, PtrComparator<std::shared_ptr<Node>>>;
    using NodeLookupMap = std::map<NodeId, std::shared_ptr<Node>>;


    template<typename T>
    struct PtrComparator {
        bool operator()(const T &lhs, const T &rhs) const {
            return (*lhs) < (*rhs);
        }
    };

    template<typename T>
    struct WeakComparator {
        PtrComparator<std::shared_ptr<T>> w;

        bool operator()(const std::weak_ptr<T> &lhs,
                        const std::weak_ptr<T> &rhs) const {
            return w.operator()(lhs.lock(), rhs.lock());
        }
    };

    class Node {
    private:
        Publication value;
        ParentSet parents;
        ChildSet children;
        typename NodeLookupMap::iterator it;

    public:
        explicit Node(NodeId id) : value(id), parents(), children() {}

        typename ParentSet::iterator add_parent(const std::shared_ptr<Node> &ptr) {
            return parents.emplace(std::weak_ptr<Node>(ptr)).first;
        }

        typename ChildSet::iterator add_child(const std::shared_ptr<Node> &ptr) {
            return children.emplace(std::shared_ptr<Node>(ptr)).first;
        }

        const Publication &get_publication() const noexcept { return value; }

        ParentSet &get_parent_set() noexcept { return parents; }

        ChildSet &get_child_set() noexcept { return children; }


        friend std::ostream &operator<<(std::ostream &os, const Node &node) {
            os << "Node {value= " << &node.value << "}";
            return os;
        }

        //virtual ~Node() { std::cout << "Destruction " << value.get_id() << std::endl; }

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

    std::vector<NodeId> to_vector(ChildSet &s)const{
        std::vector<NodeId> vec;
        vec.reserve(s.size());
        for(auto &elem: s){
            auto &drf = *elem;
            vec.push_back(drf.get_publication().get_id());
        }
        return vec;
    }

    std::vector<NodeId> to_vector_parent(ParentSet &s) const{
        std::vector<NodeId> vec;
        vec.reserve(s.size());
        for(auto &elem: s){
            auto &drf = *(elem.lock());
            vec.push_back(drf.get_publication().get_id());
        }
        return vec;
    }

public:

    explicit CitationGraph(NodeId const &stem_id) {
        publication_ids[stem_id] = std::make_shared<Node>(stem_id);
        this->source_id = stem_id;
        this->source = publication_ids[stem_id];
    }

    CitationGraph(CitationGraph<Publication> &&other) noexcept
        : publication_ids(), source(nullptr) {
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
        ChildSet &c =  find_or_throw(id)->second->get_child_set();
        return to_vector(c);
    }

    std::vector<NodeId> get_parents(NodeId const &id) const {
        ParentSet &a = find_or_throw(id)->second->get_parent_set();
        return to_vector_parent(a);
    }

    bool exists(NodeId const &id) const {
        return publication_ids.find(id) != publication_ids.end();
    }

    const Publication &operator[](NodeId const &id) const {
        return find_or_throw(id)->second->get_publication();
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

        auto added_iter = publication_ids.insert(
            publication_ids.begin(),
            std::make_pair(id, std::shared_ptr<Node>(new Node(id))));
        nl_trans.add(publication_ids, added_iter);
        std::shared_ptr<Node> &child = (*added_iter).second;
        for (NodeId parent_id : parent_ids) {
            if (parent_id == id) {
                throw PublicationNotFound();
            }
            auto parent_iter = publication_ids.find(parent_id);
            if (parent_iter == publication_ids.end()) {
                throw PublicationNotFound();
            }
            std::shared_ptr<Node> &parent = (*parent_iter).second;
            auto c_iter = parent->add_child(child);
            auto p_iter = child->add_parent(parent);
            c_trans.add(parent->get_child_set(), c_iter);
            p_trans.add(child->get_parent_set(), p_iter);
        }

        nl_trans.commit();
        p_trans.commit();
        c_trans.commit();
    }


    void add_citation(NodeId const &child_id, NodeId const &parent_id) {
        typename NodeLookupMap::iterator child_node = publication_ids.find(child_id);
        typename NodeLookupMap::iterator par_node = publication_ids.find(parent_id);
        if (child_node == publication_ids.end() ||
            par_node == publication_ids.end() || child_id == parent_id) {
            throw PublicationNotFound();
        }

        Transaction<ChildSet> c_trans;
        Transaction<ParentSet> p_trans;
        p_trans.add(child_node->second->get_parent_set(),
                    child_node->second->add_parent(par_node->second));
        c_trans.add(par_node->second->get_child_set(),
                    par_node->second->add_child(child_node->second));

        c_trans.commit();
        p_trans.commit();
    }

    template<typename T, typename U>
    void DFS(std::map<Node*, int> &visited,
            Transaction<U> &parent_set, Transaction<T> &node_lookup_map, std::shared_ptr<Node> node) {
        if (visited[node.get()] == 0) {
            node_lookup_map.add(publication_ids,
                                publication_ids.find(node->get_publication().get_id()));
            std::cout << "pub: " << node->get_publication().get_id() << std::endl;
            visited[node.get()] == -1;
            for (auto &child : node->get_child_set()) {
                ParentSet &child_parent_set = child->get_parent_set();
                std::cout << "child: " << child->get_publication().get_id() << std::endl;
                parent_set.add(child_parent_set, child_parent_set.find(node));
                DFS(visited, parent_set, node_lookup_map, child);
            }
        }
    }

    void DFS_count(std::map<Node*, int>& visited, Node* node) {
        std::cout << node->get_parent_set().size() << *node << std::endl;
        if (visited.find(node) == visited.end()) {
            visited.insert(std::make_pair(node, node->get_parent_set().size()));
/*            visited[node] =
                    node->
                    get_parent_set().size();*/
        }
        visited[node]--;
        for (auto child : node->get_child_set()) {
            DFS_count(visited, child.get());
        }
        std::cout << node->get_parent_set().size() << *node << std::endl;
    }

    void remove(NodeId const &id) {
        if (publication_ids.find(id) == publication_ids.end()) {
            throw PublicationNotFound();
        }
        if (id == source->get_publication().get_id()) {
            throw TriedToRemoveRoot();
        }

        std::cout << "remove" << std::endl;
        Transaction<NodeLookupMap> nl_trans;
        Transaction<ChildSet> c_trans;
        Transaction<ParentSet> p_trans;

        c_trans.commit();
        p_trans.commit();
        nl_trans.commit();

        typename NodeLookupMap::iterator node = publication_ids.find(id);
        std::cout << "remove2" << std::endl;

        for (auto& parent: node->second->get_parent_set()) {
            ChildSet& parent_child_set = parent.lock()->get_child_set();
            typename ChildSet::iterator iterator = parent_child_set.find(node->second);
            c_trans.add(parent_child_set, iterator);
        }
        std::cout << "remove3" << std::endl;

        ChildSet node_children = node->second->get_child_set();
        for (auto child = node_children.begin(); child != node_children.end(); ++child) {
            c_trans.add(node_children, child);
        }
        std::cout << "remove3" << std::endl;

        std::map<Node*, int> visited;
        std::cout << "remove5" << std::endl;
        DFS_count(visited, node->second.get());
        std::cout << "remove6" << std::endl;
        visited[node->second.get()] = 0;
        std::cout << "||||||||" <<std::endl;
        for (auto vis : visited) {
            std::cout << vis.first->get_publication().get_id() << ": " << vis.second << std::endl;
        }
        std::cout << "||||||||" <<std::endl;
        DFS(visited, p_trans, nl_trans, node->second);

        nl_trans.commit();
        p_trans.commit();
        c_trans.commit();
    }

    friend std::ostream &operator<<(std::ostream &os, const CitationGraph &cg) {
        for (auto &pair : cg.publication_ids) {
            os << "Children of " << pair.first << ": ";
            for(auto const &c : pair.second->get_child_set()){
                os << c->get_publication().get_id() << " ";
            }
            os << std::endl;
            os << "Parents of " << pair.first << ": ";
            for(auto const &p: pair.second->get_parent_set()){
                os << p.lock()->get_publication().get_id() << " ";
            }
            os << std::endl;
        }
        os << std::endl;
        return os;

    }
};

#endif
