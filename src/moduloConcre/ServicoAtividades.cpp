#include "ServicoAtividades.h"
#include "../../include/Regra.h"
#include "Repositorio.h"

ServicoAtividades::ServicoAtividades(){}
ServicoAtividades::~ServicoAtividades(){}

bool ServicoAtividades::registrar(const DadosAtividade& dados, Atividade& atividade, int idSessao) {
    const Sessao* sessao = session(idSessao);
    if (!sessao) 
        return false;
    const Usuario& eu = sessao->user();
    if (eu.role() != "Chefe Escotista") 
        return false;
    if (dados.title().empty() || !dados.date().valid() || !dados.validStartTime() ||dados.place().empty() || dados.type().empty() || dados.branch().empty()) 
        return false;
    for (size_t indice = 0; indice < regras_.size(); ++indice)
        if (!regras_[indice]->allows(dados)) 
            return false;
    Repositorio& repositorio = Repositorio::instance();
    Atividade nova(repositorio.newId(), dados, eu.sectionId(), "Confirmada");

    // Cadastra na atividade todos os integrantes da secao do Chefe (menos os Chefes).
    for (size_t indice = 0; indice < repositorio.usuarios.size(); ++indice) {
        const Usuario& usuario = repositorio.usuarios[indice];
        if (usuario.role() == "Jovem" && usuario.sectionId() == eu.sectionId())
            nova.enroll(usuario.id());
    }
    repositorio.atividades.push_back(nova);
    atividade = nova;
    return true;
}


void ServicoAtividades::add(const Regra<DadosAtividade>& regra) { 
    regras_.push_back(&regra); 
}
