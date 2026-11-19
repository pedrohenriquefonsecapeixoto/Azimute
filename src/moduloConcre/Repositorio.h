#ifndef REPOSITORIO_H
#define REPOSITORIO_H
#include <vector>
#include "Usuario.h"
#include "Entidades.h"
using namespace std;

/**
 * @brief Armazena em memória os dados do sistema em uma única instância compartilhada.
 */
class Repositorio {
public:

    /**
     * @brief Obtém a única instância do repositório, criando-a no primeiro acesso.
     * @return Referência à instância compartilhada de Repositorio.
     */
    static Repositorio& instance();

    /**
     * @brief Destrói o repositório e libera os recursos das suas coleções.
     */
    ~Repositorio();

    /**
     * @brief Remove todos os usuários, atividades, participações, justificativas e alertas e reinicia o próximo identificador em 100.
     */
    void clear();

    /**
     * @brief Fornece o próximo identificador e avança a sequência de identificadores.
     * @return Inteiro com o identificador gerado, começando em 100 após a criação ou limpeza do repositório.
     */
    int newId(); 

    /**
     * @brief Busca um usuário pelo identificador.
     * @param id Identificador do usuário procurado.
     * @return Ponteiro constante para o Usuario encontrado ou nullptr se ele não existir.
     */
    const Usuario* findUser(int id) const;

    /**
     * @brief Busca uma atividade pelo identificador.
     * @param id Identificador da atividade procurada.
     * @return Ponteiro constante para a Atividade encontrada ou nullptr se ela não existir.
     */
    const Atividade* findActivity(int id) const;

    /**
     * @brief Busca uma justificativa pelo identificador.
     * @param id Identificador da justificativa procurada.
     * @return Ponteiro constante para a Justificativa encontrada ou nullptr se ela não existir.
     */
    const Justificativa* findJustification(int id) const;

    /**
     * @brief Coleção de usuários armazenados em memória.
     */
    vector<Usuario> usuarios;

    /**
     * @brief Coleção de atividades armazenadas em memória.
     */
    vector<Atividade> atividades;

    /**
     * @brief Coleção de registros de presença, ausência ou atraso.
     */
    vector<Participacao> participacoes;

    /**
     * @brief Coleção de justificativas e seus estados de análise.
     */
    vector<Justificativa> justificativas;

    /**
     * @brief Coleção de alertas associados aos destinatários e às justificativas.
     */
    vector<Alerta> alertas;
private:


    /**
     * @brief Inicializa a sequência de identificadores em 100; o acesso ao construtor é privado.
     */
    Repositorio(); // Construtor privado e copia proibida: so pode existir um repositorio.
    Repositorio(const Repositorio&);
    Repositorio& operator=(const Repositorio&);
    int proximoId_;
};

#endif
