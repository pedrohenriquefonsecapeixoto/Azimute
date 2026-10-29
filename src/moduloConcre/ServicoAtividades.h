#ifndef SERVICOATIVIDADES_H
#define SERVICOATIVIDADES_H
#include "Servico.h"
#include <vector>

using namespace std;

class Sessao;
class Atividade;
class DadosAtividade;
template <class T> class Regra;

class ServicoAtividades : public Servico {
public:
    using Servico::add; // permite adicionar sessoes e regras no mesmo servico
    ServicoAtividades();
    ~ServicoAtividades();
    // Regustra atv
    bool registrar(const DadosAtividade& dados, Atividade& atividade, int idSessao);
    void add(const Regra<DadosAtividade>& regra);
private:
    ServicoAtividades(const ServicoAtividades&);
    ServicoAtividades& operator=(const ServicoAtividades&);
    vector<const Regra<DadosAtividade>*> regras_;
};
#endif 
