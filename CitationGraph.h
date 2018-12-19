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




template<typename Publication>
class CitationGraph {
private:

	class Node;
	template <typename T> struct WeakComparator;
	template <typename T> struct PtrComparator;



	using id_type = typename Publication::id_type;
	using ParentSet = std::set<std::weak_ptr<Node>, WeakComparator<Node>>;
	using ChildSet = std::set<std::shared_ptr<Node>, PtrComparator<std::shared_ptr<Node>>>;
	using NodeLookupMap = std::map<id_type, std::shared_ptr<Node>>;



	template<typename Container>
	class Transaction {
	private:
		std::vector<std::pair<Container &, typename Container::iterator>> to_be_removed{};
		bool failed;
	public:

		explicit Transaction(bool arg) : failed(arg) {};

		virtual ~Transaction() {
			if (this->failed) {
				for (auto &i: to_be_removed) {
					i.first.erase(i.second);
				}
			}
		}

		void commit() {
			failed = !failed;
		}

		void add(Container &c, typename Container::iterator &iter) {
			to_be_removed.emplace_back(c, iter);
		}
	};



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

        typename ChildSet::iterator add_child(const std::shared_ptr<Node> &ptr) {
            return children.emplace(std::shared_ptr<Node>(ptr)).first;
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

	    std::vector<id_type> get_parents() const {
		    std::vector<id_type> vec;
		    for (auto parent : parents) {
			    vec.emplace(parent->get_publication().get_id());
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



    typename NodeLookupMap::iterator find_or_throw(id_type const &id) {
	    typename NodeLookupMap::iterator node = publication_ids.find(id);
	    if (node == publication_ids.end()) {
		    throw PublicationNotFound();
	    }
	    return node;
    }

	NodeLookupMap publication_ids;
	std::shared_ptr<Node> source;

public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(id_type const &stem_id) {
        publication_ids[stem_id] = std::make_shared<Node>(stem_id);
    }

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
    // noexcept.
    CitationGraph(CitationGraph<Publication> &&other) : publication_ids(), source(nullptr) {
        *this = std::move(other);
    }

    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) {
        std::swap(this->publication_ids, std::move(other.publication_ids));
        std::swap(this->source, std::move(other.source));
        std::swap(this->source_id, std::move(other.source_id));
    }

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication.h::get_id jest noexcept. Zamiast pytajnika należy
    // wpisać stosowne wyrażenie.
    id_type get_root_id() const noexcept(Publication::get_id()) {
        return source->get_publication().get_id();
    }

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<id_type> get_children(id_type const &id) const {
	    return find_or_throw(id)->second->get_children();
    }

    // Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<id_type> get_parents(id_type const &id) const {
	    return find_or_throw(id)->second->get_parents();
    }

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(id_type const &id) const {
	    return publication_ids.find(id) != publication_ids.end();
    }

    // Zwraca referencję do obiektu reprezentującego publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
    // nie istnieje.
    Publication &operator[](id_type const &id) const {
	    return find_or_throw(id)->second->get_publication();
    }

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
    // parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
    // o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
    // któryś z wyspecyfikowanych poprzedników nie istnieje albo lista poprzedników jest pusta.
    void create(id_type const &id, id_type const &parent_id) {
        create(id, std::vector<id_type>(1, parent_id));
    }

    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
    	if (publication_ids.find(id) != publication_ids.end()) {
    		throw PublicationAlreadyCreated();
    	}

        if (parent_ids.empty()) {
            throw PublicationNotFound();
        }

	    Transaction<ChildSet> c_trans(true);
	    Transaction<ParentSet> p_trans(true);
	    Transaction<NodeLookupMap> nl_trans(true);


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


    friend std::ostream &operator<<(std::ostream &os, const CitationGraph &graph);

    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(id_type const &child_id, id_type const &parent_id) {
    	typename NodeLookupMap::iterator child_node = publication_ids.find(child_id);
    	typename NodeLookupMap::iterator par_node = publication_ids.find(parent_id);
    	if (child_node == publication_ids.end() ||
    	    par_node == publication_ids.end() ||
    	    child_id == parent_id) {
    		throw PublicationNotFound();
    	}

	    Transaction<ChildSet> c_trans(true);
	    Transaction<ParentSet> p_trans(true);
	    p_trans.add(child_node->second->get_parents(), child_node->second->add_parent(par_node->second));
	    c_trans.add(par_node->second->get_children(), par_node->second->add_child(child_node->second));

	    c_trans.commit();
	    p_trans.commit();
    }

	template <typename T>
	void DFS(Transaction<T>& iterators_set, std::shared_ptr<Node>& node) {
		iterators_set.add(publication_ids, publication_ids.find(node->get_publication().get_id()));
		for (auto child : node.get_children()) {
			DFS(iterators_set, child);
		}
	}

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
    // TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową zawierającą źródło.
    void remove(id_type const &id) {
        if (publication_ids.find(id) == publication_ids.end()) {
            throw PublicationNotFound();
        }
        if (id == source.get_id()) {
        	throw TriedToRemoveRoot();
        }

	    Transaction<ChildSet> c_trans(false);
	    Transaction<ParentSet> p_trans(false);
	    Transaction<NodeLookupMap> nl_trans(false);

	    typename NodeLookupMap::iterator node = publication_ids.find(id);

	    for (auto& parent: node->second->get_parents()) {
	    	ChildSet& parent_child_set = parent.lock()->get_children();
	    	typename ChildSet::iterator iterator = parent_child_set.find(node->second);
	    	c_trans.add(parent_child_set, iterator);
	    }

	    for (std::shared_ptr<Node>& child: node->second->get_children()) {
		    ParentSet& child_parent_set = child->get_children();
		    typename ParentSet::iterator iterator = child_parent_set.find(node->second);
		    p_trans.add(child_parent_set, iterator);
	    }

	    if (node->second->get_parents().size() == 1) {
	        DFS(nl_trans, node->second);
	    } else {
		    nl_trans.add(publication_ids, node);
	    }

	    nl_trans.commit();
	    p_trans.commit();
	    c_trans.commit();
    }
};


#endif // CITATIONGRAPH_H
