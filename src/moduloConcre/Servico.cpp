#include "Servico.h"

Servico::Servico(){}

Servico::~Servico(){}

int Servico::add(const Sessao& sessao) {
    for (int indice = 0; indice < sessoes_.size(); ++indice)
        if (sessoes_.at(indice) == &sessao) 
            return indice;
    sessoes_.add(&sessao);
    return sessoes_.size() - 1;
}

bool Servico::remove(int idSessao) {
    if (!session(idSessao)) 
        return false;
    sessoes_.set(idSessao, nullptr);
    return true;
}

const Sessao* Servico::session(int idSessao) const {
    if (idSessao < 0 || idSessao >= sessoes_.size()) 
        return nullptr;
    return sessoes_.at(idSessao);
}