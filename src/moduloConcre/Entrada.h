#ifndef ENTRADA_H
#define ENTRADA_H
#include <ctime>
#include <string>
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

#endif
