#ifndef USUARIO_H
#define USUARIO_H
#include <string>
#include "Entrada.h"
using namespace std;

class Usuario {
public:
    Usuario();
    Usuario(int id, const DadosUsuario& dados);
    Usuario(const Usuario& outro);
    Usuario& operator=(const Usuario& outro);
    ~Usuario();
    int id() const;
    const DadosUsuario& details() const;
    string role() const;
    int sectionId() const;
private:
    int id_;
    DadosUsuario dados_;
};

// Quem esta logado. Copia rasa: aponta para o mesmo Usuario, sem duplica-lo.
class Sessao {
public:
    explicit Sessao(const Usuario& usuario);
    Sessao(const Sessao& outro);
    Sessao& operator=(const Sessao& outro);
    ~Sessao();
    const Usuario& user() const;
private:
    const Usuario* usuario_;
};

#endif
