#ifndef SERVICOPRESENCA_H
#define SERVICOPRESENCA_H
#include "Servico.h"
#include "../../include/Consulta.h"

class Sessao;
class Usuario;
class Atividade;
class Participacao;

class ServicoPresenca : public Servico{
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
private:
    enum { DURACAO_MIN = 120 }; // duracao padrao da atividade, em minutos
    ServicoPresenca(const ServicoPresenca&);
    ServicoPresenca& operator=(const ServicoPresenca&);
    bool canManage(const Atividade& atividade, int idSessao) const;
    static bool contains(const Colecao<Usuario>& usuarios, int id);
    static bool isRecorded(int idAtividade, int idIntegrante);
};

#endif 