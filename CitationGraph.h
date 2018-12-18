#ifndef CITATIONGRAPH_H
#define CITATIONGRAPH_H

#include <vector>
#include <string>
#include <set>
#include <memory>
#include "Publication.h"
#include <map>

template <typename Publication>
class CitationGraph {
  private:
    class Node {
      private:
        Publication value;
        std::set<std::weak_ptr<Node>> parents;
        std::set<std::shared_ptr<Node>> children;

      public:
        Node(typename Publication::id_type id) : value(id), parents(), children() {}

        auto add_parent(const std::shared_ptr<Node>& ptr) {
          return parents.emplace(std::weak_ptr<Node>(ptr)).first;
        }

        void remove_parent(typename std::set<std::shared_ptr<Node>>::iterator it) {
          parents.erase(it);
        }

        auto add_child(const std::shared_ptr<Node>& ptr) {
          return children.emplace(std::shared_ptr<Node>(ptr)).first;
        }

        void remove_child(typename std::set<std::shared_ptr<Node>>::iterator it) {
          children.erase(it);
        }

        const Publication& get_publication() const {
          return value;
        }

        
        std::vector<typename Publication::id_type> get_children() const {
          std::vector<typename Publication::id_type> vec;
          for (auto child : children) {
            vec.emplace(child->get_publication().get_id());
          }
          return vec;
        }
    };

    std::map<typename Publication::id_type, std::shared_ptr<Node>> publication_ids;
    std::shared_ptr<Node> source;


  public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(typename Publication::id_type const &stem_id) {

    }

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
    // noexcept.
    CitationGraph(CitationGraph<Publication> &&other) : publication_ids(), source(nullptr) {
      *this = std::move(other);
    }

    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) {
      std::swap(this->publication_ids, std::move(other.publication_ids));
      std::swap(this->source, std::move(other.source));
      std::swap(this->source_id, std::move(other.source_id));
    }

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika należy
    // wpisać stosowne wyrażenie.
    typename Publication::id_type get_root_id() const noexcept(Publication::get_id()) {
      return source->get_publication().get_id();
    }

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const {
    }

    // Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const;

    // Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(typename Publication::id_type const &id) const;

    // Zwraca referencję do obiektu reprezentującego publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
    // nie istnieje.
    Publication& operator[](typename Publication::id_type const &id) const;

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
    // parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
    // o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
    // któryś z wyspecyfikowanych poprzedników nie istnieje albo lista poprzedników jest pusta.
    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id);
    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids);

    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id);

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
    // TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową zawierającą źródło.
    void remove(typename Publication::id_type const &id);
};

class PublicationAlreadyCreated : public std::exception {
    char const * what() const noexcept {
      return "PublicationAlreadyCreated";
    }
};

class PublicationNotFound : std::exception {
    char const * what() const noexcept {
      return "PublicationNotFound";
    }
};
class TriedToRemoveRoot : std::exception {
    char const * what() const noexcept {
      return "TriedToRemoveRoot";
    }
};




#endif // CITATIONGRAPH_H
