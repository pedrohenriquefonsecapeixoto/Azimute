#include "ServicoAtividades.h"
#include "../../include/Regra.h"
#include "../../include/Colecao.h"
#include "Repositorio.h"

ServicoAtividades::ServicoAtividades(const Sessao& sessao) : sessao_(&sessao) {}

ServicoAtividades::~ServicoAtividades() {}

bool ServicoAtividades::registrar(const DadosAtividade& d, Atividade& atividade) {
    const Usuario& eu = sessao_->user();
    if (eu.role() != "Chefe Escotista" || eu.sectionId() == 0) return false;
    if (d.title().empty() || !d.date().valid() || d.startTime().empty() ||
        d.place().empty() || d.type().empty() || d.branch().empty()) return false;
    for (size_t i = 0; i < regras_.size(); ++i)
        if (!regras_[i]->allows(d)) return false;

    Repositorio& r = Repositorio::instance();
    Atividade nova(r.newId(), d, eu.sectionId(), "Confirmada");
    r.atividades.push_back(nova);
    atividade = nova;
    return true;
}

Colecao<Atividade> ServicoAtividades::query(const FiltroAtividade& f) {
    Colecao<Atividade> resultado;
    const Usuario& eu = sessao_->user();
    if (eu.role() != "Chefe Escotista") return resultado;
    if (f.sectionId() != 0 && f.sectionId() != eu.sectionId()) return resultado;

    const Repositorio& r = Repositorio::instance();
    for (size_t i = 0; i < r.atividades.size(); ++i) {
        const Atividade& a = r.atividades[i];
        Data d = a.details().date();
        if (a.sectionId() != eu.sectionId()) continue;
        if (!(f.start() <= d && d <= f.end())) continue;
        if (!f.status().empty() && a.status() != f.status()) continue;
        resultado.add(a);
    }
    return resultado;
}

void ServicoAtividades::add(const Regra<DadosAtividade>& regra) { regras_.push_back(&regra); }
