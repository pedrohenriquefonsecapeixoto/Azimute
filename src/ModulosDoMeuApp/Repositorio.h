#ifndef REPOSITORIO_H
#define REPOSITORIO_H
#include <utility>
#include <vector>
#include "Usuario.h"
#include "Entrada.h"
#include "Entidades.h"

class Repositorio {
public:
    typedef std::pair<Usuario, DadosUsuario> Cadastro;
    static Repositorio& instance();
    ~Repositorio();
    void clear();
    int newId();
    const Cadastro* findUser(int id) const;
    const Atividade* findActivity(int id) const;
    const Justificativa* findJustification(int id) const;
    vector<Cadastro> usuarios;
    vector<Atividade> atividades;
    vector<Participacao> participacoes;
    vector<Justificativa> justificativas;
    vector<Alerta> alertas;
private:
    Repositorio();
    Repositorio(const Repositorio&);
    Repositorio& operator=(const Repositorio&);
    int proximoId_;
};

#endif
