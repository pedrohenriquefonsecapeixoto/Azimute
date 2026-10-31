#ifndef ENTRADA_H
#define ENTRADA_H
#include <string>
#include <ctime>
#include "Data.h"

using namespace std;



class DadosUsuario {
public:
    DadosUsuario();
    DadosUsuario(const string& nome, const Data& nascimento, const string& vinculo,
                 const string& papel, int idSecao = 0);
    DadosUsuario(const DadosUsuario& outro);
    DadosUsuario& operator=(const DadosUsuario& outro);
    ~DadosUsuario();

    string name() const;
    Data birth() const;
    string bond() const;
    string role() const;
    int sectionId() const;

private:
    string nome_;
    Data nasc_;
    string vinculo_, papel_;
    int idSecao_;
};

class DadosAtividade {
public:
    DadosAtividade();
    DadosAtividade(const string& titulo, const Data& data, const string& horario,
                   const string& local, const string& tipo, const string& ramo,
                   const string& descricao = "");
    DadosAtividade(const DadosAtividade& outro);
    DadosAtividade& operator=(const DadosAtividade& outro);
    ~DadosAtividade();

    string title() const;
    Data date() const;
    string startTime() const;
    bool validStartTime() const;
    bool endTime(int duracaoMin, time_t& fim) const;
    string place() const;
    string type() const;
    string branch() const;
    string description() const;

private:
    string titulo_;
    Data data_;
    string horario_, local_, tipo_, ramo_, descricao_;
};

class FiltroAtividade {
public:
    // situacao "" = qualquer; idSecao 0 = sem seçao
    explicit FiltroAtividade(const Data& inicio = Data(), const Data& fim = Data(),
                             const string& situacao = "", int idSecao = 0);
    FiltroAtividade(const FiltroAtividade& outro);
    FiltroAtividade& operator=(const FiltroAtividade& outro);
    ~FiltroAtividade();

    Data start() const;
    Data end() const;
    string status() const;
    int sectionId() const;

private:
    Data inicio_, fim_;
    string situacao_;
    int idSecao_;
};

class FiltroHistorico {
public:
    // idIntegrante 0 = o proprio usuario (ou a secao inteira, se idSecao != 0)
    explicit FiltroHistorico(const Data& inicio = Data(), const Data& fim = Data(),
                             int idIntegrante = 0, int idSecao = 0);
    FiltroHistorico(const FiltroHistorico& outro);
    FiltroHistorico& operator=(const FiltroHistorico& outro);
    ~FiltroHistorico();

    Data start() const;
    Data end() const;
    int memberId() const;
    int sectionId() const;

private:
    Data inicio_, fim_;
    int idIntegrante_, idSecao_;
};

#endif // AZIMUTE_SRC_MODULOCONCRE_ENTRADA_H
