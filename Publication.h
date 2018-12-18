#ifndef PUBLICATION_H
#define PUBLICATION_H

template<typename T>
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

#endif