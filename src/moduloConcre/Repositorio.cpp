#include "Repositorio.h"

Repositorio& Repositorio::instance() {
    static Repositorio unico;
    return unico;
}

Repositorio::~Repositorio() {}

void Repositorio::clear() {
    usuarios.clear();
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

Repositorio::Repositorio() : proximoId_(100) {}
