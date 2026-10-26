#ifndef ENTRADA_H
#define ENTRADA_H
#include <string>
#include "Data.h"
using namespace std;

// Dados que o cliente envia para os servicos.
class DadosUsuario {
public:
    DadosUsuario();
    DadosUsuario(const string& nome, const Data& nascimento, const string& vinculo, const string& papel, int idSecao = 0);
    DadosUsuario(const DadosUsuario& o);
    DadosUsuario& operator=(const DadosUsuario& o);
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
    DadosAtividade(const string& titulo, const Data& data, const string& horario,const string& local, const string& tipo, const string& ramo, const string& descricao = "");
    DadosAtividade(const DadosAtividade& o);
    DadosAtividade& operator=(const DadosAtividade& o);
    ~DadosAtividade();
    string title() const;
    Data date() const;
    string startTime() const;
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
    explicit FiltroAtividade(const Data& inicio = Data(), const Data& fim = Data(), const std::string& situacao = "", int idSecao = 0);
    FiltroAtividade(const FiltroAtividade& o);
    FiltroAtividade& operator=(const FiltroAtividade& o);
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
    explicit FiltroHistorico(const Data& inicio = Data(), const Data& fim = Data(), int idIntegrante = 0, int idSecao = 0);
    FiltroHistorico(const FiltroHistorico& o);
    FiltroHistorico& operator=(const FiltroHistorico& o);
    ~FiltroHistorico();
    Data start() const;
    Data end() const;
    int memberId() const;
    int sectionId() const;
private:
    Data inicio_, fim_;
    int idIntegrante_, idSecao_;
};

class FiltroAlerta {
public:
    FiltroAlerta();
    FiltroAlerta(const FiltroAlerta&);
    FiltroAlerta& operator=(const FiltroAlerta&);
    ~FiltroAlerta();
};

#endif
