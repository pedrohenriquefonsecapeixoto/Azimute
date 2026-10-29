#ifndef COLECAO_H
#define COLECAO_H
#include <vector>

using namespace std;

template <class T> class Colecao;

// percorre uma Colecao sem conhecer o dominio do cliente.
template <class T>
class Iterador {
public:
    Iterador();
    explicit Iterador(const Colecao<T>* colecao);
    Iterador(const Iterador& outro);
    Iterador& operator=(const Iterador& outro);
    ~Iterador();
    bool hasNext() const;
    const T& next();
private:
    const Colecao<T>* colecao_;
    int pos_;
};

template <class T>
class Colecao {
public:
    Colecao();
    Colecao(const Colecao& outro);
    Colecao& operator=(const Colecao& outro);
    ~Colecao();
    void add(const T& item);
    void set(int indice, const T& item);
    Iterador<T> iterator() const;
    int size() const;
    const T& at(int indice) const;
private:
    vector<T> itens_;
};

#endif 
