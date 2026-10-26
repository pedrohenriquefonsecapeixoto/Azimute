#include <iostream>
#include "../include/Colecao.h"
#include "moduloConcre/Data.h"
#include "moduloConcre/Usuario.h"
#include "moduloConcre/Entrada.h"
#include "moduloConcre/Entidades.h"
#include "moduloConcre/Repositorio.h"
#include "moduloConcre/ServicoUsuarios.h"
#include "moduloConcre/ServicoAtividades.h"
#include "moduloConcre/ServicoPresenca.h"

int main() {
    std::cout << std::boolalpha;
    Repositorio& repo = Repositorio::instance();

    // --- Dirigente (conta inicial) cadastra os usuarios ---
    Usuario dirigente(1, "Dirigente da UEL", 0);
    Sessao sessaoDir(dirigente);
    ServicoUsuarios usuarios(sessaoDir);
    Usuario carlos, ana;

    std::cout << "Cadastro Carlos: " << usuarios.registrar(
        DadosUsuario("Carlos Lima", Data(1985, 3, 10), "UEL", "Chefe Escotista", 2), carlos) << "\n";
    DadosUsuario dAna("Ana Souza", Data(2012, 5, 20), "UEL", "Jovem", 2);
    std::cout << "Cadastro Ana: " << usuarios.registrar(dAna, ana) << "\n";
    std::cout << "Cadastro Ana duplicado: " << usuarios.registrar(dAna, ana) << "\n";

    // --- Chefe cadastra atividade e registra presenca ---
    Sessao sessaoChefe(carlos);
    ServicoAtividades atividades(sessaoChefe);
    ServicoPresenca presenca(sessaoChefe);
    Atividade reuniao;

    std::cout << "Atividade cadastrada: " << atividades.registrar(
        DadosAtividade("Reuniao de Outubro", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), reuniao)
        << " (" << reuniao.status() << ")\n";
    Colecao<Atividade> lista = atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)));
    std::cout << "Atividades em outubro: " << lista.size() << "\n";

    Colecao<Usuario> integrantes = presenca.listMembers(reuniao.id());
    Colecao<Participacao> marcacoes;
    for (Iterador<Usuario> it = integrantes.iterator(); it.hasNext(); )
        marcacoes.add(Participacao(it.next().id(), "Presente"));
    std::cout << "Presenca gravada: " << presenca.record(reuniao.id(), marcacoes) << "\n";
    std::cout << "Registro duplicado: " << presenca.record(reuniao.id(), Participacao(ana.id(), "Ausente")) << "\n";

    int minutos = 0;
    bool medido = presenca.recordingTime(reuniao.id(), minutos);
    std::cout << "Tempo ate o registro: " << medido << " (" << minutos << " min)\n";

    // --- Alerta de justificativa pendente (rotina interna) ---
    // Ainda nao existe servico de justificativa, entao inserimos direto no repositorio.
    repo.justificativas.push_back(Justificativa(repo.newId(), ana.id(), reuniao.id(), 2));
    Usuario sistema(0, "Sistema", 0);
    Sessao sessaoSis(sistema);
    ServicoPresenca rotina(sessaoSis);
    std::cout << "Rotina de alertas: " << rotina.generatePendingAlerts() << "\n";
    std::cout << "Alertas do Carlos: " << presenca.query(FiltroAlerta()).size() << "\n";

    // --- Jovem consulta o proprio historico ---
    Sessao sessaoAna(ana);
    ServicoPresenca presencaAna(sessaoAna);
    FiltroHistorico filtro(Data(2026, 10, 1), Data(2026, 10, 31));
    double freq = 0;
    std::cout << "Historico da Ana: " << presencaAna.query(filtro).size() << " registro(s), frequencia "
              << presencaAna.attendancePercentage(filtro, freq) << " " << freq << "%\n";

    FiltroHistorico outro(Data(2026, 10, 1), Data(2026, 10, 31), carlos.id());
    std::cout << "Ana ve outro integrante: " << presencaAna.query(outro).size() << " registro(s), percentual "
              << presencaAna.attendancePercentage(outro, freq) << "\n";

    // --- Dirigente acompanha a secao inteira ---
    ServicoPresenca presencaDir(sessaoDir);
    FiltroHistorico secao(Data(2026, 10, 1), Data(2026, 10, 31), 0, 2);
    std::cout << "Secao 2: " << presencaDir.query(secao).size() << " registro(s), frequencia "
              << presencaDir.attendancePercentage(secao, freq) << " " << freq << "%\n";

    return 0;
}
