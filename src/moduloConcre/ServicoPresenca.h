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

class ServicoPresenca : public Servico, public Consulta<Participacao, FiltroHistorico>,
                        public Consulta<Alerta, FiltroAlerta> {
public:
    ServicoPresenca();
    ~ServicoPresenca();
    // retorna os jovens de uma turma
    Colecao<Usuario> listMembers(int idAtividade, int idSessao) const;
    bool record(int idAtividade, const Participacao& participacao, int idSessao);
    // Grava presença toda ou nada: se um item for invalido, nenhum e gravado.
    bool record(int idAtividade, const Colecao<Participacao>& lista, int idSessao);
    // Minutos entre o fim da atividade e o primeiro registro.
    bool recordingTime(int idAtividade, int& minutos, int idSessao) const;
    Colecao<Participacao> query(const FiltroHistorico& filtro, int idSessao) override;
    // (Presente + Atraso) / total * 100. Retorna false se negado ou sem registros.
    bool attendancePercentage(const FiltroHistorico& filtro, double& percentual, int idSessao);
    // Rotina interna. Nao duplica alertas.
    bool generatePendingAlerts(int idSessao);
    // Alertas do usuario da sessao cuja justificativa ainda esta pendente.
    Colecao<Alerta> query(const FiltroAlerta&, int idSessao) override;
private:
    enum { DURACAO_MIN = 120 }; // duracao padrao da atividade, em minutos
    ServicoPresenca(const ServicoPresenca&);
    ServicoPresenca& operator=(const ServicoPresenca&);
    bool canManage(const Atividade& atividade, int idSessao) const;
    static bool contains(const Colecao<Usuario>& usuarios, int id);
    static bool isRecorded(int idAtividade, int idIntegrante);
    static bool alertExists(int idDestinatario, int idJustificativa);
    // Regras de acesso 
    // Jovem ve so o proprio; Chefe ve a propria secao; Dirigente ve todos.
    bool collect(const FiltroHistorico& filtro, Colecao<Participacao>& saida, int idSessao) const;
};

#endif 