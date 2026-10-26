#ifndef CONSULTA_H
#define CONSULTA_H

template <class T> class Colecao;

// T = tipo do resultado, F = tipo do filtro.
template <class T, class F>
class Consulta {
public:
    Consulta() {}
    virtual ~Consulta() {}

    virtual Colecao<T> query(const F& filtro) = 0; // vazia se negado ou sem resultado

private:
    // Copia proibida: declarada e nunca implementada.
    Consulta(const Consulta&);
    Consulta& operator=(const Consulta&);
};

#endif 
