#ifndef SERVICOUSUARIOS_H
#define SERVICOUSUARIOS_H
#include "Servico.h"
#include <vector>
using namespace std;

class Sessao;
class Usuario;
class DadosUsuario;
template <class T> class Regra;
class ServicoUsuarios : public Servico {
public:
    using Servico::add; // permite adicionar sessoes 
    ServicoUsuarios();
    ~ServicoUsuarios();

    // Apenas o Dirigente cadastra.
    bool registrar(const DadosUsuario& dados, Usuario& usuario, int idSessao);

    void add(const Regra<DadosUsuario>& regra);

private:
    // Copia proibida: o servico controla sua propria lista de sessoes.
    ServicoUsuarios(const ServicoUsuarios&);
    ServicoUsuarios& operator=(const ServicoUsuarios&);
    vector<const Regra<DadosUsuario>*> regras_;
};

#endif 
