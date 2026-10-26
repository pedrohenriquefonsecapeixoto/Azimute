#ifndef SERVICOATIVIDADES_H
#define SERVICOATIVIDADES_H
#include <vector>
#include "../../include/Consulta.h"
using namespace std;

class Sessao;
class Atividade;
class DadosAtividade;
class FiltroAtividade;
template <class T> class Regra;

class ServicoAtividades : public Consulta<Atividade, FiltroAtividade> {
public:
    explicit ServicoAtividades(const Sessao& sessao);
    ~ServicoAtividades();
    bool registrar(const DadosAtividade& d, Atividade& atividade);
    Colecao<Atividade> query(const FiltroAtividade& f) override;
    void add(const Regra<DadosAtividade>& regra);

private:
    ServicoAtividades(const ServicoAtividades&);
    ServicoAtividades& operator=(const ServicoAtividades&);
    const Sessao* sessao_;
    vector<const Regra<DadosAtividade>*> regras_;
};

#endif
