#include "Repositorio.h"

Repositorio& Repositorio::instance() {
    static Repositorio unico;
    return unico;
}

Repositorio::~Repositorio() {}

void Repositorio::clear() {
    usuarios.clear();
    atividades.clear();
    participacoes.clear();
    proximoId_ = 100;
}

int Repositorio::newId() { 
    return proximoId_++; 
}

const Usuario* Repositorio::findUser(int id) const {
    for (size_t indice = 0; indice < usuarios.size(); ++indice)
        if (usuarios[indice].id() == id) 
            return &usuarios[indice];
    return nullptr;
}

const Atividade* Repositorio::findActivity(int id) const {
    for (size_t indice = 0; indice < atividades.size(); ++indice)
        if (atividades[indice].id() == id) 
            return &atividades[indice];
    return nullptr;
}

Repositorio::Repositorio() : proximoId_(100) {}
