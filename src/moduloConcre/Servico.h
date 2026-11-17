#ifndef SERVICO_H
#define SERVICO_H
#include "../../include/Colecao.h"

class Sessao;

class Servico {
public:
    Servico();
    virtual ~Servico();
    int add(const Sessao& sessao);
    bool remove(int idSessao);
protected:
    const Sessao* session(int idSessao) const;

private:
    Servico(const Servico&);
    Servico& operator=(const Servico&);
    Colecao<const Sessao*> sessoes_;
};
#endif 
