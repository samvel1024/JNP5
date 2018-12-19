#ifndef PUBLICATION_H
#define PUBLICATION_H



template <int percent_throw>
class PublicationId {

    int id;

public:

    PublicationId(): id(0) {}
    PublicationId(int id) : id(id) {}
    PublicationId(const PublicationId &p) : id(p.id){}
    PublicationId(const PublicationId &&p) noexcept : id(p.id) {}

    bool operator==(const PublicationId &rhs) const {
        return id == rhs.id;
    }

    bool operator!=(const PublicationId &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const PublicationId &rhs) const {
        return id < rhs.id;
    }

    bool operator>(const PublicationId &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const PublicationId &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const PublicationId &rhs) const {
        return !(*this < rhs);
    }


};



template<typename T, int percent_throw>
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