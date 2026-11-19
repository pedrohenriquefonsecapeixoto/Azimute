#ifndef ENTIDADES_H
#define ENTIDADES_H

#include <string>
#include <vector>
#include "Entrada.h"

using namespace std;

// Entidades guardadas no Repositorio.

/**
 * @brief Representa uma atividade ou evento de uma seção e os integrantes inscritos.
 */
class Atividade {
public:
    /**
     * @brief Cria uma atividade com identificadores de atividade e seção iguais a zero.
     */
    Atividade();
    /**
     * @brief Cria uma atividade com os dados, a seção e a situação informados.
     * @param id Identificador da atividade.
     * @param dados Dados da atividade, como título, data, horário e local.
     * @param idSecao Identificador da seção.
     * @param situacao Situação atribuída à atividade, como Confirmada, Realizada ou Cancelada.
     */
    Atividade(int id, const DadosAtividade& dados, int idSecao, const string& situacao);
    /**
     * @brief Cria uma cópia de outra atividade, incluindo seus integrantes.
     * @param outro Atividade cujos dados serão copiados.
     */
    Atividade(const Atividade& outro);
    /**
     * @brief Copia os dados e os integrantes de outra atividade para esta atividade.
     * @param outro Atividade cujos dados serão copiados.
     * @return Referência a esta Atividade após a atribuição.
     */
    Atividade& operator=(const Atividade& outro);

    /**
     * @brief Destrói a atividade e libera os recursos dos seus atributos.
     */
    ~Atividade();

    /**
     * @brief Consulta o identificador da atividade.
     * @return Inteiro que identifica a atividade; 
     */
    int id() const;

    /**
     * @brief Consulta os dados da atividade.
     * @return Referência constante ao objeto DadosAtividade armazenado nesta atividade.
     */
    const DadosAtividade& details() const;

     /**
     * @brief Consulta a seção à qual a atividade pertence.
     * @return Inteiro com o identificador da seção.
     */
    int sectionId() const;

    /**
     * @brief Consulta a situação da atividade.
     * @return String com a situação armazenada, como Confirmada, Realizada ou Cancelada.
     */
    string status() const; // Confirmada, Realizada, Cancelada
    
    /**
     * @brief Consulta os identificadores dos integrantes inscritos na atividade.
     * @return Referência constante ao vetor de identificadores; o vetor estará vazio se não houver inscrições.
     */
    const vector<int>& members() const; // ids dos integrantes cadastrados na atividade
    
    /**
     * @brief Adiciona um identificador à lista de integrantes da atividade.
     * @param idIntegrante Identificador do integrante a ser adicionado.
     * @throws bad_alloc Se faltar memória para ampliar a lista de integrantes.
     */
    void enroll(int idIntegrante);
private:
    int id_;
    DadosAtividade dados_;
    int idSecao_;
    string situacao_;
    vector<int> integrantes_;
};


/**
 * @brief Representa a presença, ausência ou atraso de um integrante em uma atividade.
 */
class Participacao {
public:

    /**
     * @brief Cria uma participação ainda sem atividade e sem horário de registro associados.
     * @param idIntegrante Identificador do integrante; zero por padrão.
     * @param situacao Situação informada, como Presente, Ausente ou Atraso; vazia por padrão.
     */
    explicit Participacao(int idIntegrante = 0, const string& situacao = "");
    
    /**
     * @brief Cria uma cópia de outra participação.
     * @param outro Participação cujos dados serão copiados.
     */
    Participacao(const Participacao& outro);


    /**
     * @brief Copia os dados de outra participação para esta participação.
     * @param outro Participação cujos dados serão copiados.
     * @return Referência a esta Participacao após a atribuição.
     */
    Participacao& operator=(const Participacao& outro);
    
    
    /**
     * @brief Destrói o objeto de participação.
     */
    ~Participacao();
    
    
    /**
     * @brief Consulta o integrante ao qual a participação pertence.
     * @return Inteiro com o identificador do integrante.
     */
    int memberId() const;
    
    
    /**
     * @brief Consulta a situação de participação do integrante.
     * @return String com a situação armazenada, como Presente, Ausente ou Atraso.
     */
    string status() const; // Presente, Ausente, Atraso
    
    
    /**
     * @brief Consulta a atividade associada à participação.
     * @return Inteiro com o identificador da atividade; zero enquanto não houver associação.
     */
    int activityId() const;
    
    
    /**
     * @brief Consulta o instante em que a participação foi registrada.
     * @return Valor time_t com o instante associado; zero enquanto o registro não tiver sido associado.
     */
    time_t recordedAt() const;
    
    
    /**
     * @brief Associa a participação a uma atividade e a um instante de registro.
     * @param idAtividade Identificador da atividade.
     * @param quando Instante do registro, representado por time_t.
     */
    void attach(int idAtividade, time_t quando);
private:
    int idAtividade_, idIntegrante_;
    string situacao_;
    time_t registradoEm_;
};

/**
 * @brief Representa a justificativa de um integrante para uma atividade e seu estado de análise.
 */
class Justificativa {
public:
    /**
     * @brief Cria uma justificativa com os identificadores e o estado informados.
     * @param id Identificador da justificativa; zero por padrão.
     * @param idIntegrante Identificador do integrante; zero por padrão.
     * @param idAtividade Identificador da atividade; zero por padrão.
     * @param idSecao Identificador da seção; zero por padrão.
     * @param estado Estado da justificativa; Pendente por padrão.
     */
    explicit Justificativa(int id = 0, int idIntegrante = 0, int idAtividade = 0, int idSecao = 0,
                           const string& estado = "Pendente");
        
    /**
     * @brief Cria uma cópia de outra justificativa.
     * @param outro Justificativa cujos dados serão copiados.
     */
    Justificativa(const Justificativa& outro);
    
    /**
     * @brief Cria uma cópia de outra justificativa.
     * @param outro Justificativa cujos dados serão copiados.
     */
    Justificativa& operator=(const Justificativa& outro);
    
    /**
     * @brief Destrói o objeto de justificativa.
     */
    ~Justificativa();

     /**
     * @brief Consulta o identificador da justificativa.
     * @return Inteiro que identifica a justificativa.
     */
    int id() const;

    /**
     * @brief Consulta a seção associada à justificativa.
     * @return Inteiro com o identificador da seção.
     */
    int sectionId() const;

    /**
     * @brief Consulta o estado de análise da justificativa.
     * @return String com o estado armazenado, como Pendente, Aceita ou Recusada.
     */
    string status() const; // Pendente, Aceita, Recusada
    
    /**
     * @brief Substitui o estado de análise da justificativa pelo valor informado.
     * @param estado Novo estado a ser armazenado, como Pendente, Aceita ou Recusada.
     */
    void setStatus(const string& estado);
private:
    int id_, idIntegrante_, idAtividade_, idSecao_;
    string estado_;
};

/**
 * @brief Relaciona um destinatário a uma justificativa para permitir a consulta de alertas pendentes.
 */
class Alerta {
public:
/**
     * @brief Cria um alerta com o destinatário e a justificativa informados.
     * @param idDestinatario Identificador do usuário que receberá o alerta; zero por padrão.
     * @param idJustificativa Identificador da justificativa associada; zero por padrão.
     */
    explicit Alerta(int idDestinatario = 0, int idJustificativa = 0);
    
    /**
     * @brief Cria uma cópia de outro alerta.
     * @param outro Alerta cujos dados serão copiados.
     */
    Alerta(const Alerta& outro);
    
    /**
     * @brief Copia os dados de outro alerta para este alerta.
     * @param outro Alerta cujos dados serão copiados.
     * @return Referência a este Alerta após a atribuição.
     */
    Alerta& operator=(const Alerta& outro);

    
    /**
     * @brief Destrói o objeto de alerta.
     */
    ~Alerta();

    /**
     * @brief Consulta o destinatário do alerta.
     * @return Inteiro com o identificador do destinatário.
     */
    int recipientId() const;
    /**
     * @brief Consulta a justificativa associada ao alerta.
     * @return Inteiro com o identificador da justificativa.
     */
    int justificationId() const;
private:
    int idDest_, idJust_;
};

#endif
