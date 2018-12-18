#include <iostream>
#include "CitationGraph.h"
#include <set>
#include <memory>

template <typename T>
class Publication {
private:
    T id;

public:
    typedef T id_type;

    Publication(T arg) : id(arg) {}

    T get_id() const {
        return id;
    };
};


class Dummy {
private:
    int id;

public:

    Dummy(int arg) : id(arg) {}

    bool operator<(const Dummy& lhs) const {
      return id < lhs.id;
    }
};

/*bool operator<(const std::weak_ptr<Dummy>& lhs, const std::weak_ptr<Dummy>& rhs) {
  return lhs.lock()->operator<(*rhs.lock());
}*/

int main() {
  //std::set<std::weak_ptr<Dummy>> set_weak;
  std::set<std::shared_ptr<Dummy>> set_shared;

  auto ptr2 = std::make_shared<Dummy>(2);
  auto ptr3 = std::make_shared<Dummy>(3);

/*  set_weak.emplace(ptr2);
  set_weak.emplace(ptr3);*/

  set_shared.emplace(ptr2);
  set_shared.emplace(ptr3);



  //CitationGraph<Publication<int>> graph(23);



    return 0;
}