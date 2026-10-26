#ifndef ENTIDADES_H
#define ENTIDADES_H
#include <ctime>
#include <string>
#include "Entrada.h"
using namespace std;

// Entidades no Repositorio.
class Atividade {
public:
    Atividade();
    Atividade(int id, const DadosAtividade& dados, int idSecao, const string& situacao);
    Atividade(const Atividade& o);
    Atividade& operator=(const Atividade& o);
    ~Atividade();
    int id() const;
    const DadosAtividade& details() const;
    int sectionId() const;
    string status() const;
private:
    int id_;
    DadosAtividade dados_;
    int idSecao_;
    string situacao_;
};

class Participacao {
public:
    explicit Participacao(int idIntegrante = 0, const string& situacao = "");
    Participacao(const Participacao& o);
    Participacao& operator=(const Participacao& o);
    ~Participacao();
    int memberId() const;
    string status() const;
    int activityId() const;
    time_t recordedAt() const;
    void attach(int idAtividade, time_t quando);
private:
    int idAtividade_, idIntegrante_;
    string situacao_;
    time_t registradoEm_;
};

class Justificativa {
public:
    explicit Justificativa(int id = 0, int idIntegrante = 0, int idAtividade = 0, int idSecao = 0, const string& estado = "Pendente");
    Justificativa(const Justificativa& o);
    Justificativa& operator=(const Justificativa& o);
    ~Justificativa();
    int id() const;
    int sectionId() const;
    string status() const;
    void setStatus(const string& estado);
private:
    int id_, idIntegrante_, idAtividade_, idSecao_;
    string estado_;
};

class Alerta {
public:
    explicit Alerta(int idDestinatario = 0, int idJustificativa = 0);
    Alerta(const Alerta& o);
    Alerta& operator=(const Alerta& o);
    ~Alerta();
    int recipientId() const;
    int justificationId() const;
private:
    int idDest_, idJust_;
};

#endif