#ifndef SERVICO_H
#define SERVICO_H
#include "../../include/Colecao.h"

class Sessao;

/**
 * @brief Classe base dos servicos do sistema.
 * Mantem a lista de sessoes autorizadas a usar o servico.
 */
class Servico {
public:
    /** @brief Cria um servico sem nenhuma sessao registrada. */
    Servico();

    /** @brief Destrutor virtual. */
    virtual ~Servico();

    /**
     * @brief Registra uma sessao no servico.
     * @param sessao Sessao a ser registrada.
     * @return Identificador da sessao no servico; se ja estava registrada, retorna o id existente.
     */    
    int add(const Sessao& sessao);

    /**
     * @brief Remove uma sessao do servico.
     * @param idSessao Identificador da sessao retornado por add().
     * @return true se a sessao existia e foi removida, false se o id e invalido ou ja foi removido.
     */    
    bool remove(int idSessao);
protected:
    /**
     * @brief Busca uma sessao registrada pelo identificador.
     * @param idSessao Identificador da sessao retornado por add().
     * @return Ponteiro para a sessao, ou nullptr se o id e invalido ou a sessao foi removida.
     */
    const Sessao* session(int idSessao) const;

private:
    /** @brief Copia proibida. */
    Servico(const Servico&);

    /**
     * @brief Atribuicao proibida.
     * @param outro Servico que seria atribuido.
     * @return Referencia para o proprio objeto.
     */    
    Servico& operator=(const Servico&);

    Colecao<const Sessao*> sessoes_;
};
#endif 
