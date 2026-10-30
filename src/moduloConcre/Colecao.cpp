#include "../../include/Colecao.h"
#include "Usuario.h"
#include "Entidades.h"
#include <stdexcept>

template <class T>
Iterador<T>::Iterador() : colecao_(nullptr), pos_(0) {}

template <class T>
Iterador<T>::Iterador(const Colecao<T>* colecao) : colecao_(colecao), pos_(0) {}

template <class T>
Iterador<T>::Iterador(const Iterador<T>& outro) : colecao_(outro.colecao_), pos_(outro.pos_) {}

template <class T>
Iterador<T>& Iterador<T>::operator=(const Iterador<T>& outro) {
    if (this != &outro) {
        colecao_ = outro.colecao_;
        pos_ = outro.pos_;
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
    if (!hasNext()) throw out_of_range("Iterador::next sem elemento");
    return colecao_->at(pos_++);
}

template <class T>
Colecao<T>::Colecao() {}

template <class T>
Colecao<T>::Colecao(const Colecao<T>& outro) : itens_(outro.itens_) {}

template <class T>
Colecao<T>& Colecao<T>::operator=(const Colecao<T>& outro) {
    if (this != &outro) itens_ = outro.itens_;
    return *this;
}

template <class T>
Colecao<T>::~Colecao() {}

template <class T>
void Colecao<T>::add(const T& item) {
    itens_.push_back(item);
}

template <class T>
void Colecao<T>::set(int indice, const T& item) {
    itens_.at(indice) = item;
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
const T& Colecao<T>::at(int indice) const {
    return itens_.at(indice);
}

// Especializacao usada pelo controle de sessoes desta feature.
template class Colecao<const Sessao*>;
template class Iterador<const Sessao*>;
template class Colecao<Usuario>;
template class Iterador<Usuario>;
template class Colecao<Participacao>;
template class Iterador<Participacao>;
template class Colecao<Atividade>;
template class Iterador<Atividade>;
