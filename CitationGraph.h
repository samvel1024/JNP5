#ifndef CITATIONGRAPH_H
#define CITATIONGRAPH_H

#include <vector>
#include <string>
#include "Publication.h"

template <typename Publication>
class CitationGraph {
  public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(typename Publication::id_type const &stem_id);

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
    // noexcept.
    CitationGraph(CitationGraph<Publication> &&other);
    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other);

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
    // wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika należy
    // wpisać stosowne wyrażenie.
    typename Publication::id_type get_root_id() const; //noexcept(?);

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
    // identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
    // nie istnieje.
    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const;

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


#endif // CITATIONGRAPH_H
