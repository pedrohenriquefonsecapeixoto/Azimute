#include "ServicoPresenca.h"
#include <limits>
#include <set>
#include <ctime>
#include <string>
#include <vector>
#include "../../include/Colecao.h"
#include "Repositorio.h"

using namespace std;

ServicoPresenca::ServicoPresenca(){}
ServicoPresenca::~ServicoPresenca(){}

Colecao<Usuario> ServicoPresenca::listMembers(int idAtividade, int idSessao) const {
    Colecao<Usuario> membros;
    const Repositorio& repositorio = Repositorio::instance();
    const Atividade* atividade = repositorio.findActivity(idAtividade);
    if (!atividade || !canManage(*atividade, idSessao)) 
        return membros;
    const vector<int>& ids = atividade->members();
    for (size_t indice = 0; indice < ids.size(); ++indice) {
        const Usuario* cadastro = repositorio.findUser(ids[indice]);
        if (cadastro && cadastro->role() == "Jovem" && cadastro->sectionId() == atividade->sectionId())
            membros.add(*cadastro);
    }
    return membros;
}

bool ServicoPresenca::record(int idAtividade, const Participacao& participacao, int idSessao) { //registrar uma pessoa
    Colecao<Participacao> lista;
    lista.add(participacao);
    return record(idAtividade, lista, idSessao);
}

bool ServicoPresenca::record(int idAtividade, const Colecao<Participacao>& lista, int idSessao) { //registrar uma lista
    Repositorio& repositorio = Repositorio::instance();
    const Atividade* atividade = repositorio.findActivity(idAtividade);
    if (!atividade || !canManage(*atividade, idSessao) || atividade->status() == "Cancelada") 
        return false;
    time_t fim;
    if (lista.size() == 0 || !atividade->details().endTime(DURACAO_MIN, fim) || time(nullptr) < fim)
        return false; // registra somente depois do fim da atividade
    Colecao<Usuario> membros = listMembers(idAtividade, idSessao);
    time_t agora = time(nullptr);
    vector<Participacao> pendentes;
    set<int> integrantes;
    for (Iterador<Participacao> iterador = lista.iterator(); iterador.hasNext(); ) {
        Participacao copia = iterador.next();
        string situacao = copia.status();
        if (situacao != "Presente" && situacao != "Ausente" && situacao != "Atraso")
            return false;
        if (!contains(membros, copia.memberId())) //se aquele aluno nao é um aluno cadastrado naquela atividade nao deixa
            return false;
        if (isRecorded(idAtividade, copia.memberId())) //verifica se ja tem participação naquela atividade 
            return false;
        if (!integrantes.insert(copia.memberId()).second) 
            return false;
        copia.attach(idAtividade, agora);
        pendentes.push_back(copia);
    }
    vector<Participacao> completas = repositorio.participacoes;
    completas.insert(completas.end(), pendentes.begin(), pendentes.end());
    repositorio.participacoes.swap(completas);
    return true;
}

bool ServicoPresenca::recordingTime(int idAtividade, int& minutos, int idSessao) const {
    const Repositorio& repositorio = Repositorio::instance();
    const Atividade* atividade = repositorio.findActivity(idAtividade);
    if (!atividade || !canManage(*atividade, idSessao)) 
        return false;
    time_t primeiro = 0;
    for (size_t indice = 0; indice < repositorio.participacoes.size(); ++indice) {
        const Participacao& participacao = repositorio.participacoes[indice];
        if (participacao.activityId() != idAtividade) 
            continue;
        if (primeiro == 0 || participacao.recordedAt() < primeiro) 
            primeiro = participacao.recordedAt();
    }
    if (primeiro == 0) 
        return false;
    time_t fim;
    if (!atividade->details().endTime(DURACAO_MIN, fim)) 
        return false;
    double diferenca = difftime(primeiro, fim) / 60;
    if (diferenca > numeric_limits<int>::max()) 
        return false;
    minutos = diferenca < 0 ? 0 : static_cast<int>(diferenca);
    return true;
}

Colecao<Participacao> ServicoPresenca::query(const FiltroHistorico& filtro, int idSessao) {
    Colecao<Participacao> resultado;
    collect(filtro, resultado, idSessao);
    return resultado;
}

bool ServicoPresenca::attendancePercentage(const FiltroHistorico& filtro, double& percentual, int idSessao) {
    Colecao<Participacao> registros;
    if (!collect(filtro, registros, idSessao) || registros.size() == 0) 
        return false;
    int comparecimentos = 0;
    for (Iterador<Participacao> iterador = registros.iterator(); iterador.hasNext(); ) {
        string situacao = iterador.next().status();
        if (situacao == "Presente" || situacao == "Atraso") ++comparecimentos;
    }
    percentual = 100.0 * comparecimentos / registros.size();
    return true;
}

bool ServicoPresenca::canManage(const Atividade& atividade, int idSessao) const {
    const Sessao* sessao = session(idSessao);
    if (!sessao) 
        return false;
    const Usuario& solicitante = sessao->user();
    return solicitante.role() == "Chefe Escotista" && solicitante.sectionId() == atividade.sectionId();
}

bool ServicoPresenca::contains(const Colecao<Usuario>& usuarios, int id) {
    for (Iterador<Usuario> iterador = usuarios.iterator(); iterador.hasNext(); )
        if (iterador.next().id() == id) 
            return true;
    return false;
}

bool ServicoPresenca::isRecorded(int idAtividade, int idIntegrante) {
    const Repositorio& repositorio = Repositorio::instance();
    for (size_t indice = 0; indice < repositorio.participacoes.size(); ++indice) {
        const Participacao& participacao = repositorio.participacoes[indice];
        if (participacao.activityId() == idAtividade && participacao.memberId() == idIntegrante) 
            return true;
    }
    return false;
}

bool ServicoPresenca::collect(const FiltroHistorico& filtro, Colecao<Participacao>& saida, int idSessao) const {
    const Sessao* sessao = session(idSessao);
    if (!sessao) 
        return false;
    const Usuario& eu = sessao->user();
    const Repositorio& repositorio = Repositorio::instance();
    string papel = eu.role();
    if (papel != "Jovem" && papel != "Chefe Escotista" && papel != "Dirigente da UEL") 
        return false;
    bool porSecao = (filtro.memberId() == 0 && filtro.sectionId() != 0);
    int alvo = (filtro.memberId() == 0) ? eu.id() : filtro.memberId();
    if (porSecao) {
        if (papel == "Jovem") 
            return false;
        if (papel == "Chefe Escotista" && filtro.sectionId() != eu.sectionId()) 
            return false;
    } else if (papel == "Jovem") {            //por alvo
        if (alvo != eu.id()) 
            return false;
    } else {
        const Usuario* aluno = repositorio.findUser(alvo);
        if (!aluno || aluno->role() != "Jovem") 
            return false;   //so pode consultar Jovem
        if (papel == "Chefe Escotista" && aluno->sectionId() != eu.sectionId()) 
            return false; //aluno de outra turma
    }
    for (size_t indice = 0; indice < repositorio.participacoes.size(); ++indice) {
        const Participacao& participacao = repositorio.participacoes[indice];
        const Atividade* atividade = repositorio.findActivity(participacao.activityId());
        if (!atividade || atividade->status() == "Cancelada") 
            continue;
        Data data = atividade->details().date();
        if (!(filtro.start() <= data && data <= filtro.end())) 
            continue;
        if (porSecao) {
            const Usuario* cadastro = repositorio.findUser(participacao.memberId());
            if (!cadastro || cadastro->sectionId() != filtro.sectionId()) 
                continue;
        } else if (participacao.memberId() != alvo) {
            continue;
        }
        saida.add(participacao);
    }
    return true;
}
