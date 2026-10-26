#include "../../include/Colecao.h"
#include "Usuario.h"
#include "Entidades.h"


template <class T>
Iterador<T>::Iterador() : colecao_(nullptr), pos_(0) {}

template <class T>
Iterador<T>::Iterador(const Colecao<T>* colecao) : colecao_(colecao), pos_(0) {}

template <class T>
Iterador<T>::Iterador(const Iterador<T>& o) : colecao_(o.colecao_), pos_(o.pos_) {}

template <class T>
Iterador<T>& Iterador<T>::operator=(const Iterador<T>& o) {
    if (this != &o) {
        colecao_ = o.colecao_;
        pos_ = o.pos_;
    }
    return *this;
}

template <class T>
Iterador<T>::~Iterador() {}

template <class T>
bool Iterador<T>::hasNext() const {
    return colecao_ && pos_ < colecao_->size();
}

template <class T>
const T& Iterador<T>::next() {
    return colecao_->at(pos_++);
}

template <class T>
Colecao<T>::Colecao() {}

template <class T>
Colecao<T>::Colecao(const Colecao<T>& o) : itens_(o.itens_) {}

template <class T>
Colecao<T>& Colecao<T>::operator=(const Colecao<T>& o) {
    if (this != &o) itens_ = o.itens_;
    return *this;
}

template <class T>
Colecao<T>::~Colecao() {}

template <class T>
void Colecao<T>::add(const T& item) {
    itens_.push_back(item);
}

template <class T>
Iterador<T> Colecao<T>::iterator() const {
    return Iterador<T>(this);
}

template <class T>
int Colecao<T>::size() const {
    return (int)itens_.size();
}

template <class T>
const T& Colecao<T>::at(int i) const {
    return itens_[i];
}


template class Colecao<int>;
template class Iterador<int>;
template class Colecao<Usuario>;
template class Iterador<Usuario>;
template class Colecao<Atividade>;
template class Iterador<Atividade>;
template class Colecao<Participacao>;
template class Iterador<Participacao>;
template class Colecao<Alerta>;
template class Iterador<Alerta>;
