#ifndef CITATIONGRAPH_H
#define CITATIONGRAPH_H

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <map>
#include <ostream>
#include <iostream>

#ifdef DEBUG
#define LOG(x) x;
#else
#define LOG(x)
#endif


class PublicationAlreadyCreated : public std::exception {
    char const *what() const noexcept {
        return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : std::exception {
    char const *what() const noexcept {
        return "PublicationNotFound";
    }
};

class TriedToRemoveRoot : std::exception {
    char const *what() const noexcept {
        return "TriedToRemoveRoot";
    }
};


template<typename Container>
class Transaction {
private:
    std::vector<std::pair<Container &, typename Container::iterator>> to_be_removed{};
    bool failed = true;
public:

    explicit Transaction() = default;

    virtual ~Transaction() {
        if (this->failed) {
            for (auto &i: to_be_removed) {
                i.first.erase(i.second);
            }
        }
    }

    void commit() {
        failed = false;
    }

    void add(Container &c, typename Container::iterator &iter) {
        to_be_removed.emplace_back(c, iter);
    }


};

template<typename Publication>
class CitationGraph {
private:


    template<typename T>
    struct PtrComparator {
        bool operator()(const T &lhs, const T &rhs) const {
            return (*lhs) < (*rhs);
        }
    };

    template<typename T>
    struct WeakComparator {
        PtrComparator<std::shared_ptr<T>> w;

        bool operator()(const std::weak_ptr<T> &lhs, const std::weak_ptr<T> &rhs) const {
            return w.operator()(lhs.lock(), rhs.lock());
        }
    };

    class Node;
    using id_type = typename Publication::id_type;
    using ParentSet = std::set<std::weak_ptr<Node>, WeakComparator<Node>>;
    using ChildSet = std::set<std::shared_ptr<Node>, PtrComparator<std::shared_ptr<Node>>>;

    class Node {


    private:
        Publication value;
        ParentSet parents;
        ChildSet children;

    public:
        Node(id_type id) : value(id), parents(), children() {}


        typename ParentSet::iterator add_parent(const std::shared_ptr<Node> &ptr) {
            return parents.emplace(std::weak_ptr<Node>(ptr)).first;
        }

        void remove_parent(typename std::set<std::shared_ptr<Node>>::iterator it) {
            parents.erase(it);
        }

        typename ChildSet::iterator add_child(const std::shared_ptr<Node> &ptr) {
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

        bool operator<(const Node &rhs) const {
            return this->value.get_id() < rhs.value.get_id();
        }
    };

    using NodeLookupMap = std::map<id_type, std::shared_ptr<Node>>;
    NodeLookupMap publication_ids;
    std::shared_ptr<Node> source;


public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(id_type const &stem_id) {
      try {
        source = std::make_shared<Node>(stem_id); // thorws

        // If an exception is thrown by any operation,// this function has no effect.
        publication_ids.emplace(std::make_pair<>(stem_id, source));
      } catch (...) {
        source = nullptr;
        publication_ids.clear(); // nothrow, possibly unnecessary
        throw;
      }
    }

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
    // noexcept.
    CitationGraph(CitationGraph<Publication> &&other) : publication_ids(), source(nullptr) {
        *this = std::move(other); // nothrow
    }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) {
        std::swap(this->publication_ids, std::move(other.publication_ids));
        std::swap(this->source, std::move(other.source));
        std::swap(this->source_id, std::move(other.source_id));
    }

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika należy
    // wpisać stosowne wyrażenie.
    id_type get_root_id() const noexcept(Publication::get_id()) {
        return source->get_publication().get_id();
    }

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<id_type> get_children(id_type const &id) const {
      if (publication_ids.find(id) == publication_ids.end()) {
        throw PublicationNotFound();
      }
      return publication_ids[id]->get_children();
    }

    // Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<id_type> get_parents(id_type const &id) const {
      if (publication_ids.find(id) == publication_ids.end()) {
        throw PublicationNotFound();
      }
      return publication_ids[id]->get_parents();
    }

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(id_type const &id) const {
    }

    // Zwraca referencję do obiektu reprezentującego publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
    // nie istnieje.
    Publication &operator[](id_type const &id) const;

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
    // parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
    // o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
    // któryś z wyspecyfikowanych poprzedników nie istnieje albo lista poprzedników jest pusta.
    void create(id_type const &id, id_type const &parent_id) {
        create(id, std::vector<id_type>(1, parent_id));
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {

        if (parent_ids.empty()) {
            throw PublicationNotFound();
        }

        Transaction<ChildSet> c_trans;
        Transaction<ParentSet> p_trans;
        Transaction<NodeLookupMap> nl_trans;


        auto added_iter = publication_ids.insert(publication_ids.begin(), std::make_pair(
            id, std::shared_ptr<Node>(new Node(id))));
        nl_trans.add(publication_ids, added_iter);
        std::shared_ptr<Node> &child = (*added_iter).second;
        for (id_type parent_id: parent_ids) {
            if (parent_id == id) {
                throw PublicationNotFound(); // TODO Probably other exception should be thrown
            }
            auto parent_iter = publication_ids.find(parent_id);
            if (parent_iter == publication_ids.end()) {
                throw PublicationNotFound();
            }
            std::shared_ptr<Node> &parent = (*parent_iter).second;
            auto c_iter = parent->add_child(child);
            auto p_iter = child->add_parent(parent);
            c_trans.add(parent->get_children(), c_iter);
            p_trans.add(child->get_parents(), p_iter);
        }


        nl_trans.commit();
        p_trans.commit();
        c_trans.commit();

    }

    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(id_type const &child_id, id_type const &parent_id);

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
    // TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową zawierającą źródło.
    void remove(id_type const &id) {


    }
};

#endif // CITATIONGRAPH_H
