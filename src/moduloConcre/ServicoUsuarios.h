#ifndef SERVICOUSUARIOS_H
#define SERVICOUSUARIOS_H
#include "Servico.h"
#include <vector>
using namespace std;

class Sessao;
class Usuario;
class DadosUsuario;
template <class T> class Regra;

/**
 * @brief Gerencia o cadastro de usuários pelo Dirigente da UEL, aplicando as validações e as regras adicionais do cliente.
 */
class ServicoUsuarios : public Servico {
public:
    using Servico::add; // permite adicionar sessoes 
    
    /**
     * @brief Cria o serviço de usuários sem regras adicionais cadastradas.
     */
    ServicoUsuarios();

    /**
     * @brief Destrói o serviço sem destruir as sessões nem as regras externas referenciadas.
     */
    ~ServicoUsuarios();

    // Apenas o Dirigente cadastra.
    /**
     * @brief Cadastra um usuário quando a sessão do Dirigente, os dados e as regras permitem a operação.
     * @param dados Dados de nome, nascimento, vínculo, papel e seção do usuário.
     * @param usuario Objeto Usuario preenchido com o cadastro criado quando a operação tem sucesso.
     * @param idSessao Identificador da sessão do Dirigente da UEL previamente adicionada a este serviço.
     * @return true se o usuário for cadastrado; false se a sessão não existir, o solicitante não for Dirigente, os dados forem inválidos, houver cadastro com o mesmo nome e nascimento ou uma regra adicional recusar o cadastro.
     * @throws std::bad_alloc Se faltar memória durante a operação; a exceção é propagada para quem chamou o serviço.
     */
    bool registrar(const DadosUsuario& dados, Usuario& usuario, int idSessao);


    /**
     * @brief Adiciona uma regra externa que será consultada nos próximos cadastros.
     * @param regra Regra de validação dos dados; deve permanecer válida enquanto for utilizada pelo serviço.
     * @throws std::bad_alloc Se faltar memória para armazenar a referência à regra.
     */
    void add(const Regra<DadosUsuario>& regra);

private:
    // Copia proibida: o servico controla sua propria lista de sessoes.
    ServicoUsuarios(const ServicoUsuarios&);
    ServicoUsuarios& operator=(const ServicoUsuarios&);
    vector<const Regra<DadosUsuario>*> regras_;
};

#endif 
