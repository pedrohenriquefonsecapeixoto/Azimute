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
    justificativas.clear();
    alertas.clear();
    proximoId_ = 100;
}

int Repositorio::newId() { return proximoId_++; }

const Repositorio::Cadastro* Repositorio::findUser(int id) const {
    for (size_t i = 0; i < usuarios.size(); ++i)
        if (usuarios[i].first.id() == id) return &usuarios[i];
    return nullptr;
}

const Atividade* Repositorio::findActivity(int id) const {
    for (size_t i = 0; i < atividades.size(); ++i)
        if (atividades[i].id() == id) return &atividades[i];
    return nullptr;
}

const Justificativa* Repositorio::findJustification(int id) const {
    for (size_t i = 0; i < justificativas.size(); ++i)
        if (justificativas[i].id() == id) return &justificativas[i];
    return nullptr;
}

Repositorio::Repositorio() : proximoId_(100) {}
