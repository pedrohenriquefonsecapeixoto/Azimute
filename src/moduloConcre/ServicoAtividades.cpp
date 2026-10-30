#include "ServicoAtividades.h"
#include "../../include/Regra.h"
#include "../../include/Colecao.h"
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


Colecao<Atividade> ServicoAtividades::query(const FiltroAtividade& filtro, int idSessao) {
    Colecao<Atividade> resultado;
    const Sessao* sessao = session(idSessao);
    if (!sessao) 
        return resultado;
    const Usuario& eu = sessao->user();
    if (eu.role() != "Chefe Escotista") 
        return resultado;
    if (filtro.sectionId() != 0 && filtro.sectionId() != eu.sectionId()) 
        return resultado;
    const Repositorio& repositorio = Repositorio::instance();
    for (size_t indice = 0; indice < repositorio.atividades.size(); ++indice) {
        const Atividade& atividade = repositorio.atividades[indice];
        Data data = atividade.details().date();
        if (atividade.sectionId() != eu.sectionId()) 
            continue;
        if (!(filtro.start() <= data && data <= filtro.end())) 
            continue;
        if (!filtro.status().empty() && atividade.status() != filtro.status()) 
            continue;
        resultado.add(atividade);
    }
    return resultado;
}

