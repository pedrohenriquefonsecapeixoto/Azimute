#ifndef SERVICOPRESENCA_H
#define SERVICOPRESENCA_H
#include "Servico.h"
#include "../../include/Consulta.h"

class Sessao;
class Usuario;
class Atividade;
class Participacao;
class Alerta;
class FiltroHistorico;
class FiltroAlerta;

/**
  @brief Serviço responsável pelo controle de presença dos jovens nas atividades.
  Não possui atributos próprios; usa a constante DURACAO_MIN (duração padrão de uma atividade, em minutos).
 
  Permite que o Chefe Escotista registre a presença da turma, consulte o histórico
  de participações, calcule o percentual de frequência e gere alertas de
  justificativas pendentes. Também funciona como Consulta de Participacao
  (filtrada por FiltroHistorico) e de Alerta (filtrada por FiltroAlerta).
 */
class ServicoPresenca : public Servico, public Consulta<Participacao, FiltroHistorico>,
                        public Consulta<Alerta, FiltroAlerta> {
public:
    /**
      @brief Construtor padrão. Cria o serviço de presença sem nenhuma sessão registrada.
     */
    ServicoPresenca();

    /**
      @brief Destrutor.
     */
    ~ServicoPresenca();

    /**
      @brief Lista os jovens que fazem parte de uma atividade (a turma que será chamada).
     
      Só devolve resultado se a sessão for de um Chefe Escotista da mesma seção da atividade.
     
      @param idAtividade Identificador da atividade.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return Coleção com os jovens da atividade; vazia se a atividade não existir ou o usuário não tiver permissão.
      @throw std::bad_alloc Se faltar memória.
     */
    Colecao<Usuario> listMembers(int idAtividade, int idSessao) const;

    /**
      @brief Registra a presença de um único jovem em uma atividade.
     
      @param idAtividade Identificador da atividade.
      @param participacao Participação (jovem e situação: Presente, Ausente ou Atraso) que será registrada.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se a presença foi registrada; false se o registro foi recusado
              (veja as regras em record(int, const Colecao<Participacao>&, int)).
      @throw std::bad_alloc Se faltar memória.
     */
    bool record(int idAtividade, const Participacao& participacao, int idSessao);

    /**
      @brief Registra a presença de vários jovens de uma vez, de forma "tudo ou nada".
     
      Se qualquer item for inválido, nenhum é gravado. O registro é recusado quando:
      a atividade não existe, está cancelada ou ainda não terminou; o usuário não é o
      Chefe Escotista da seção; a lista está vazia; a situação não é Presente, Ausente ou Atraso;
      o jovem não pertence à atividade; ou o jovem já tem presença registrada (ou aparece repetido na lista).
     
      @param idAtividade Identificador da atividade.
      @param lista Participações (uma por jovem) que serão registradas.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se todas as presenças foram gravadas; false se nada foi gravado.
      @throw std::bad_alloc Se faltar memória.
     */
    bool record(int idAtividade, const Colecao<Participacao>& lista, int idSessao);

    /**
      @brief Calcula quantos minutos se passaram entre o fim da atividade e o primeiro registro de presença.
     
      @param idAtividade Identificador da atividade.
      @param[out] minutos Recebe o tempo em minutos (0 se o registro foi feito antes do fim previsto).
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se o cálculo foi feito; false se a atividade não existe, o usuário
              não tem permissão ou ainda não há presença registrada.
     */
    bool recordingTime(int idAtividade, int& minutos, int idSessao) const;

    /**
      @brief Consulta o histórico de participações conforme um filtro.
     
      Regras de acesso: o Jovem vê só o próprio histórico; o Chefe Escotista vê a própria seção;
      o Dirigente da UEL vê todos. Atividades canceladas não entram no resultado.
     
      @param filtro Filtro do histórico (jovem ou seção e período de datas).
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return Participações encontradas; vazia se não houver nenhuma ou se o acesso for negado.
      @throw std::bad_alloc Se faltar memória.
     */
    Colecao<Participacao> query(const FiltroHistorico& filtro, int idSessao) override;

    /**
      @brief Calcula o percentual de presença: (Presente + Atraso) / total * 100.
     
      Usa as mesmas regras de acesso de query(const FiltroHistorico&, int).
     
      @param filtro Filtro do histórico (jovem ou seção e período de datas).
      @param[out] percentual Recebe o percentual de presença, de 0 a 100.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se o percentual foi calculado; false se o acesso foi negado ou não há registros.
      @throw std::bad_alloc Se faltar memória.
     */
    bool attendancePercentage(const FiltroHistorico& filtro, double& percentual, int idSessao);

    /**
      @brief Gera os alertas das justificativas pendentes (rotina interna do sistema).
     
      Cada justificativa pendente gera um alerta para os Chefes Escotistas da sua seção.
      Não duplica alertas que já existem.
     
      @param idSessao Identificador da sessão; precisa ser a do usuário de papel "Sistema".
      @return true se a rotina foi executada; false se a sessão for inválida ou não for do sistema.
      @throw std::bad_alloc Se faltar memória.
     */
    bool generatePendingAlerts(int idSessao);

    /**
      @brief Consulta os alertas do usuário da sessão cuja justificativa ainda está pendente.
     
      @param filtro Filtro de alertas (não utilizado: o resultado depende apenas do usuário da sessão).
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return Alertas pendentes do usuário; vazia se a sessão for inválida ou não houver alertas.
      @throw std::bad_alloc Se faltar memória.
     */
    Colecao<Alerta> query(const FiltroAlerta& filtro, int idSessao) override;

private:
    enum { DURACAO_MIN = 120 };

    /**
      @brief Construtor de cópia desabilitado (o serviço não pode ser copiado).
     */
    ServicoPresenca(const ServicoPresenca&);

    /**
      @brief Atribuição desabilitada (o serviço não pode ser atribuído).
     */
    ServicoPresenca& operator=(const ServicoPresenca&);

    /**
      @brief Verifica se o usuário da sessão pode gerenciar a presença de uma atividade.
     
      @param atividade Atividade que se deseja gerenciar.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se o usuário é Chefe Escotista da mesma seção da atividade; false caso contrário.
     */
    bool canManage(const Atividade& atividade, int idSessao) const;

    /**
      @brief Verifica se um usuário está presente em uma coleção de usuários.
     
      @param usuarios Coleção onde será feita a busca.
      @param id Identificador do usuário procurado.
      @return true se o usuário foi encontrado.
     */
    static bool contains(const Colecao<Usuario>& usuarios, int id);

    /**
      @brief Verifica se um jovem já tem presença registrada em uma atividade.
     
      @param idAtividade Identificador da atividade.
      @param idIntegrante Identificador do jovem.
      @return true se já existe um registro para esse jovem nessa atividade.
     */
    static bool isRecorded(int idAtividade, int idIntegrante);

    /**
      @brief Verifica se já existe um alerta de uma justificativa para um destinatário.
     
      @param idDestinatario Identificador do usuário que recebe o alerta.
      @param idJustificativa Identificador da justificativa que originou o alerta.
      @return true se o alerta já existe (usado para não duplicar alertas).
     */
    static bool alertExists(int idDestinatario, int idJustificativa);

    /**
      @brief Aplica as regras de acesso e reúne as participações pedidas por um filtro.
     
      Jovem vê só o próprio; Chefe vê a própria seção; Dirigente vê todos.
      Atividades canceladas e participações fora do período são ignoradas.
     
      @param filtro Filtro do histórico (jovem ou seção e período de datas).
      @param[out] saida Coleção que recebe as participações encontradas.
      @param idSessao Identificador da sessão do usuário que faz a requisição.
      @return true se o acesso foi permitido (mesmo que nada tenha sido encontrado); false se foi negado.
     */
    bool collect(const FiltroHistorico& filtro, Colecao<Participacao>& saida, int idSessao) const;
};

#endif
