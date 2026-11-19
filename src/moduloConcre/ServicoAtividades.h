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


/**
 * @brief Servico de atividades escoteiras.
 *
 * Permite que Chefes Escotistas registrem atividades para a sua secao e
 * consultem as atividades ja cadastradas. As atividades so sao aceitas se
 * passarem em todas as Regra adicionadas ao servico.
 */
class ServicoAtividades : public Servico, public Consulta<Atividade, FiltroAtividade> {
public:
    using Servico::add; // permite adicionar sessoes e regras no mesmo servico

    /** @brief Cria o servico sem nenhuma regra registrada. */
    ServicoAtividades();

    /** @brief Destrutor. */
    ~ServicoAtividades();

    /**
     * @brief Registra uma nova atividade para a secao do Chefe logado.
     * A atividade nasce "Confirmada" e todos os jovens da secao do Chefe sao inscritos nela.
     * @param dados Dados da atividade (titulo, data, horario, local, tipo e ramo sao obrigatorios).
     * @param atividade Parametro de saida que recebe a atividade criada; so e alterado em caso de sucesso.
     * @param idSessao Identificador da sessao de quem esta registrando (deve ser Chefe Escotista).
     * @return true se a atividade foi registrada; false se a sessao e invalida, o usuario nao e
     *         Chefe Escotista, os dados sao invalidos ou alguma regra nao permitiu.
     */    
    bool registrar(const DadosAtividade& dados, Atividade& atividade, int idSessao);

    /**
     * @brief Consulta as atividades da secao do Chefe logado.
     * @param filtro Criterios de busca (periodo, situacao e secao).
     * @param idSessao Identificador da sessao de quem esta consultando (deve ser Chefe Escotista).
     * @return Colecao com as atividades encontradas; vazia se a sessao e invalida, o usuario nao
     *         e Chefe Escotista ou o filtro pede outra secao.
     */
    Colecao<Atividade> query(const FiltroAtividade& filtro, int idSessao) override;

    /**
     * @brief Adiciona uma regra de validacao para o registro de atividades.
     * @param regra Regra a ser aplicada em registrar().
     */    
    void add(const Regra<DadosAtividade>& regra);
private:
    /** @brief Copia proibida. */
    ServicoAtividades(const ServicoAtividades&);

    /**
     * @brief Atribuicao proibida.
     * @param outro ServicoAtividades que seria atribuido.
     * @return Referencia para o proprio objeto.
     */    
    ServicoAtividades& operator=(const ServicoAtividades&);
    vector<const Regra<DadosAtividade>*> regras_;
};
#endif 
