#ifndef ENTRADA_H
#define ENTRADA_H
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

#endif
