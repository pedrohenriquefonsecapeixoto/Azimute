#ifndef SERVICOATIVIDADES_H
#define SERVICOATIVIDADES_H
#include "Servico.h"
#include <vector>
#include "../../include/Consulta.h"

using namespace std;

class Sessao;
class Atividade;
class DadosAtividade;
class FiltroAtividade;
template <class T> class Regra;

class ServicoAtividades : public Servico, public Consulta<Atividade, FiltroAtividade> {
public:
    using Servico::add; // permite adicionar sessoes e regras no mesmo servico
    ServicoAtividades();
    ~ServicoAtividades();
    // Registra atv
    bool registrar(const DadosAtividade& dados, Atividade& atividade, int idSessao);
    Colecao<Atividade> query(const FiltroAtividade& filtro, int idSessao) override;
    void add(const Regra<DadosAtividade>& regra);
private:
    ServicoAtividades(const ServicoAtividades&);
    ServicoAtividades& operator=(const ServicoAtividades&);
    vector<const Regra<DadosAtividade>*> regras_;
};
#endif 
