#ifndef REPOSITORIO_H
#define REPOSITORIO_H
#include <vector>
#include "Usuario.h"
#include "Entidades.h"
using namespace std;

class Repositorio {
public:
    static Repositorio& instance();
    ~Repositorio();
    void clear();
    int newId(); 
    const Usuario* findUser(int id) const;
    const Atividade* findActivity(int id) const;
    vector<Usuario> usuarios;
    vector<Atividade> atividades;
    vector<Participacao> participacoes;
private:
    Repositorio(); // Construtor privado e copia proibida: so pode existir um repositorio.
    Repositorio(const Repositorio&);
    Repositorio& operator=(const Repositorio&);
    int proximoId_;
};

#endif
