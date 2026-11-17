#include "ServicoUsuarios.h"
#include "../../include/Regra.h"
#include "Repositorio.h"

using namespace std;

ServicoUsuarios::ServicoUsuarios(){}

ServicoUsuarios::~ServicoUsuarios(){}

bool ServicoUsuarios::registrar(const DadosUsuario& dados, Usuario& usuario, int idSessao) {
    const Sessao* sessao = session(idSessao);
    if (!sessao) 
        return false;
    if (sessao->user().role() != "Dirigente da UEL") 
        return false;
    if (dados.name().empty() || !dados.birth().valid() || dados.bond().empty()) 
        return false;
    if (dados.role() != "Jovem" && dados.role() != "Chefe Escotista" && dados.role() != "Dirigente da UEL") 
        return false;
    Repositorio& repositorio = Repositorio::instance();
    for (size_t indice = 0; indice < repositorio.usuarios.size(); ++indice) {
        const DadosUsuario& existente = repositorio.usuarios[indice].details();
        if (existente.name() == dados.name() && existente.birth() == dados.birth()) 
            return false;
    }
    for (size_t indice = 0; indice < regras_.size(); ++indice)
        if (!regras_[indice]->allows(dados)) 
            return false;
    Usuario novo(repositorio.newId(), dados);
    repositorio.usuarios.push_back(novo);
    usuario = novo;
    return true;
}

void ServicoUsuarios::add(const Regra<DadosUsuario>& regra) { 
    regras_.push_back(&regra); 
}
