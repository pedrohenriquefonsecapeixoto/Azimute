#ifndef COLECAO_H
#define COLECAO_H
#include <vector>
using namespace std;

template <class T> class Colecao;
//iterador pra qualquer tipo
template <class T>
class Iterador {
public:
    Iterador();
    explicit Iterador(const Colecao<T>* colecao);
    Iterador(const Iterador& o);
    Iterador& operator=(const Iterador& o);
    ~Iterador();

    bool hasNext() const;
    const T& next();

private:
    const Colecao<T>* colecao_;
    int pos_;
};

//coleção de qualquer tipo
template <class T>
class Colecao {
public:
    Colecao();
    Colecao(const Colecao& o);
    Colecao& operator=(const Colecao& o);
    ~Colecao();

    void add(const T& item);
    Iterador<T> iterator() const;
    int size() const;
    const T& at(int i) const; //pra acessar um indice da coleção especifico

private:
    vector<T> itens_;
};

#endif 
