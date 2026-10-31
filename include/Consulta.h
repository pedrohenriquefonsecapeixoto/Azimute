#ifndef CONSULTA_H
#define CONSULTA_H

template <class T> class Colecao;

// Contrato de consulta: T = tipo do resultado, F = tipo do filtro.
template <class T, class F>
class Consulta {
public:
    Consulta() {}
    virtual ~Consulta() {}
    virtual Colecao<T> query(const F& filtro, int idSessao) = 0; 
private:
    Consulta(const Consulta&);
    Consulta& operator=(const Consulta&);
};

#endif 
