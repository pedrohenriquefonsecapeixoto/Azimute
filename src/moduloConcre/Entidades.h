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

class Participacao {
public:
    explicit Participacao(int idIntegrante = 0, const string& situacao = "");
    Participacao(const Participacao& outro);
    Participacao& operator=(const Participacao& outro);
    ~Participacao();
    int memberId() const;
    string status() const; // Presente, Ausente, Atraso
    int activityId() const;
    time_t recordedAt() const;
    void attach(int idAtividade, time_t quando);
private:
    int idAtividade_, idIntegrante_;
    string situacao_;
    time_t registradoEm_;
};

#endif
