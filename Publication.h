#ifndef PUBLICATION_H
#define PUBLICATION_H


#include <ostream>

using namespace std;
class ComparisonException : public std::exception {
    char const *what() const noexcept override { return "PublicationAlreadyCreated"; }
};

class PublicationId {

    int id;
    static int exception_prob;

public:

    PublicationId(const int id) : id(id) {}

    PublicationId(const PublicationId &p) : id(p.id) {}

    PublicationId(PublicationId &&p) : id(p.id) {};

    bool operator<(const PublicationId &rhs) const {
        if (rand() % 100 < exception_prob) {
            throw ComparisonException();
        }
        return id < rhs.id;
    }

    bool operator==(const PublicationId &rhs) const {
        return id == rhs.id;
    }

    static void set_exception_prob(int p){
        PublicationId::exception_prob = p;
    }


    friend std::ostream &operator<<(ostream &os, const PublicationId &id) {
        os << id.id;
        return os;
    }


};

int PublicationId::exception_prob = 0;


template<typename T>
class Publication {
public:
    typedef T id_type;

    Publication(id_type const &_id) : id(_id) {
    }

    id_type get_id() const noexcept {
        return id;
    }

private:
    id_type id;
};

#endif