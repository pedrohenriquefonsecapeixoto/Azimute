#ifndef USUARIO_H
#define USUARIO_H
#include <string>
#include "Entrada.h"
using namespace std;

/**
 * @brief Usuario do sistema, identificado por um id e com seus dados cadastrais.
 */
class Usuario {
public:
    /** @brief Cria um usuario vazio, com id igual a 0. */
    Usuario();

    /**
     * @brief Cria um usuario com id e dados cadastrais.
     * @param id Identificador unico do usuario.
     * @param dados Dados cadastrais do usuario.
     */    
    Usuario(int id, const DadosUsuario& dados);

    /**
     * @brief Construtor de copia.
     * @param outro Usuario a ser copiado.
     */    
    Usuario(const Usuario& outro);

    /**
     * @brief Operador de atribuicao.
     * @param outro Usuario cujo conteudo sera copiado.
     * @return Referencia para o proprio objeto.
     */    
    Usuario& operator=(const Usuario& outro);

    /** @brief Destrutor. */   
    ~Usuario();

    /**
     * @brief Obtem o identificador do usuario.
     * @return Id do usuario.
     */   
    int id() const;

    /**
     * @brief Obtem os dados cadastrais do usuario.
     * @return Referencia constante para os dados do usuario.
     */
    const DadosUsuario& details() const;

    /**
     * @brief Obtem o papel do usuario no sistema.
     * @return Papel do usuario (ex.: "Chefe Escotista" ou "Jovem").
     */    
    string role() const;

    /**
     * @brief Obtem o identificador da secao do usuario.
     * @return Id da secao.
     */    
    int sectionId() const;
private:
    int id_;
    DadosUsuario dados_;
};

/**
 * @brief Representa quem esta logado no sistema.
 * a Sessao apenas aponta para o Usuario, sem duplica-lo.
 * O Usuario precisa continuar existindo enquanto a Sessao for usada.
 */
class Sessao {
public:
    /**
     * @brief Cria uma sessao para um usuario.
     * @param usuario Usuario logado.
     */
    explicit Sessao(const Usuario& usuario);

    /**
     * @brief Construtor de copia.
     * @param outro Sessao a ser copiada; a nova sessao aponta para o mesmo Usuario.
     */
    Sessao(const Sessao& outro);

    /**
     * @brief Operador de atribuicao
     * @param outro Sessao cujo Usuario sera apontado por esta
     * @return Referencia para o proprio objeto
     */
    Sessao& operator=(const Sessao& outro);

    /** @brief Destrutor. */
    ~Sessao();

    /**
     * @brief Obtem o usuario logado.
     * @return Referencia constante para o Usuario da sessao.
     */
    const Usuario& user() const;
private:
    const Usuario* usuario_;
};

#endif
