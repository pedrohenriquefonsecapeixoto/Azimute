#ifndef ENTIDADES_H
#define ENTIDADES_H

#include <string>
#include <vector>
#include "Entrada.h"

using namespace std;

// Entidades guardadas no Repositorio.

class Atividade {
public:
    Atividade();
    Atividade(int id, const DadosAtividade& dados, int idSecao, const string& situacao);
    Atividade(const Atividade& outro);
    Atividade& operator=(const Atividade& outro);
    ~Atividade();
    int id() const;
    const DadosAtividade& details() const;
    int sectionId() const;
    string status() const; // Confirmada, Realizada, Cancelada
    const vector<int>& members() const; // ids dos integrantes cadastrados na atividade
    void enroll(int idIntegrante);
private:
    int id_;
    DadosAtividade dados_;
    int idSecao_;
    string situacao_;
    vector<int> integrantes_;
};


#endif
