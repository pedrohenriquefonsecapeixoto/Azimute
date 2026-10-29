#ifndef REGRA_H
#define REGRA_H

//regra que o cliente pode plugar nos servicos.
template <class T>
class Regra {
public:
    Regra() {}
    virtual ~Regra() {}
    virtual bool allows(const T& dados) const = 0; // true = aceita
private:
    // Copia proibida: declarada e nunca implementada.
    Regra(const Regra&);
    Regra& operator=(const Regra&);
};

#endif 
