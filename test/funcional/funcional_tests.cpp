#include "funcional_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
#include "../../src/moduloConcre/ServicoAtividades.h"
#include "../../src/moduloConcre/Repositorio.h"
#include "../../src/moduloConcre/ServicoPresenca.h"
#include <string>
#include <ctime>
#include <vector>
#include "../../include/Colecao.h"

using namespace std;

// ---------- Cadastrar usuários ----------

static int falhas = 0;

static void verify(bool condicao, const string& descricao) {
    if (!condicao) 
        ++falhas;
    cout << (condicao ? "[OK] " : "[FALHA] ") << descricao << '\n';
}

static bool igual(const Usuario& a, const Usuario& b) {
    return a.id() == b.id() && a.details().name() == b.details().name()
        && a.details().birth() == b.details().birth() && a.details().bond() == b.details().bond()
        && a.role() == b.role() && a.sectionId() == b.sectionId();
}

// Cadastrar usuario, CT-01: dirigente em sessao, Ana sem cadastro anterior.
static void testCT01CadastroValido() {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    Usuario dirigente(1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoDirigente(dirigente);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    DadosUsuario dados("Ana Souza", Data(2012, 5, 20), "UEL", "Jovem", 2);
    Usuario nova;
    bool ok = usuarios.registrar(dados, nova, idDirigenteUsuarios);
    verify(ok && nova.id() >= 100 && nova.role() == "Jovem" && nova.sectionId() == 2,
        "CT-01: cadastro retorna true, id novo, papel Jovem e secao 2");
    const Usuario* salvo = repositorio.findUser(nova.id());
    verify(salvo && igual(nova, *salvo) && nova.details().name() == "Ana Souza"
        && nova.details().birth() == Data(2012, 5, 20) && nova.details().bond() == "UEL"
        && repositorio.usuarios.size() == 1, "CT-01: usuario completo foi armazenado");
}

// Cadastrar usuario, CT-02: mesmo nome e mesma data de nascimento.
static void testCT02CadastroDuplicado() {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    Usuario dirigente(1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoDirigente(dirigente);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    DadosUsuario dados("Ana Souza", Data(2012, 5, 20), "UEL", "Jovem", 2);
    Usuario cadastrada;
    bool preparado = usuarios.registrar(dados, cadastrada, idDirigenteUsuarios);
    Usuario original(77, DadosUsuario("Usuario anterior", Data(1990, 3, 10), "Outro vinculo", "Chefe Escotista", 3));
    Usuario nova = original;
    bool ok = usuarios.registrar(dados, nova, idDirigenteUsuarios);
    verify(preparado && !ok, "CT-02: cadastro duplicado retorna false");
    verify(preparado && repositorio.usuarios.size() == 1 && igual(repositorio.usuarios[0], cadastrada)
        && igual(nova, original), "CT-02: nenhum novo usuario e criado; cadastro existente e toda a saida permanecem iguais");
}


// ---------- Cadastrar atividade/evento ----------

static bool igualAtv(const Atividade& a, const Atividade& b) {
    return a.id() == b.id() && a.details().title() == b.details().title()
        && a.details().date() == b.details().date() && a.details().startTime() == b.details().startTime()
        && a.details().place() == b.details().place() && a.details().type() == b.details().type()
        && a.details().branch() == b.details().branch() && a.sectionId() == b.sectionId()
        && a.status() == b.status() && a.members() == b.members();
}

// Chefe da secao 2; Jovens 10 e 11 na secao 2, Jovem 12 na secao 3.
static void prepararSecoes(const Usuario& chefe) {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    repositorio.usuarios.push_back(chefe);
    repositorio.usuarios.push_back(Usuario(10, DadosUsuario("Jovem A", Data(2012, 1, 1), "UEL", "Jovem", 2)));
    repositorio.usuarios.push_back(Usuario(11, DadosUsuario("Jovem B", Data(2012, 2, 2), "UEL", "Jovem", 2)));
    repositorio.usuarios.push_back(Usuario(12, DadosUsuario("Jovem C", Data(2012, 3, 3), "UEL", "Jovem", 3)));
}

// Cadastrar atividade, CT-01: Chefe da secao 2 com dados completos.
static void testAtvCT01CadastroValido() {
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    prepararSecoes(chefe);
    Sessao sessaoChefe(chefe);
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    DadosAtividade dados("Acampamento de Primavera", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "Escoteiro");
    Atividade nova;
    bool ok = atividades.registrar(dados, nova, idChefe);
    verify(ok && nova.id() >= 100 && nova.status() == "Confirmada" && nova.sectionId() == 2,
        "CT-01: cadastro retorna true, id proprio, situacao Confirmada e secao 2");
    const Atividade* salva = Repositorio::instance().atividades.empty() ? nullptr : &Repositorio::instance().atividades[0];
    verify(salva && igualAtv(nova, *salva) && Repositorio::instance().atividades.size() == 1
        && nova.details().title() == "Acampamento de Primavera" && nova.details().startTime() == "08:30",
        "CT-01: atividade completa foi armazenada");
    verify(nova.members().size() == 2 && nova.members()[0] == 10 && nova.members()[1] == 11,
        "CT-01: Jovens da secao do Chefe inscritos, sem pedido ao Dirigente");
}

// Cadastrar atividade, CT-02: campo obrigatorio vazio (sem titulo).
static void testAtvCT02SemTitulo() {
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    prepararSecoes(chefe);
    Sessao sessaoChefe(chefe);
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    DadosAtividade dados("", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "Escoteiro");
    Atividade original(77, DadosAtividade("Anterior", Data(2026, 1, 1), "10:00", "Outro", "Reuniao", "Lobinho"), 3, "Realizada");
    Atividade nova = original;
    bool ok = atividades.registrar(dados, nova, idChefe);
    verify(!ok, "CT-02: cadastro sem titulo retorna false");
    verify(Repositorio::instance().atividades.empty() && igualAtv(nova, original),
        "CT-02: nenhuma atividade e criada e a saida permanece igual");
}

// Cadastrar atividade: horario invalido.
static void testAtvHorarioInvalido() {
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    prepararSecoes(chefe);
    Sessao sessaoChefe(chefe);
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    const string horarios[] = {"25:00", "8:00"};
    for (const string& horario : horarios) {
        DadosAtividade dados("Reuniao", Data(2026, 11, 15), horario, "Sede", "Reuniao", "Escoteiro");
        Atividade nova;
        verify(!atividades.registrar(dados, nova, idChefe) && Repositorio::instance().atividades.empty(),
            "horario invalido \"" + horario + "\" retorna false e nao cria atividade");
    }
}

// Cadastrar atividade: sessao que nao e de Chefe.
static void testAtvSessaoNaoChefe() {
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    prepararSecoes(chefe);
    Usuario jovem(10, DadosUsuario("Jovem A", Data(2012, 1, 1), "UEL", "Jovem", 2));
    Sessao sessaoJovem(jovem);
    ServicoAtividades atividades;
    int idJovem = atividades.add(sessaoJovem);
    DadosAtividade dados("Reuniao", Data(2026, 11, 15), "08:30", "Sede", "Reuniao", "Escoteiro");
    Atividade nova;
    verify(!atividades.registrar(dados, nova, idJovem) && Repositorio::instance().atividades.empty(),
        "sessao de Jovem retorna false e nao cria atividade");
}

class RegraSemAcampamento : public Regra<DadosAtividade> {
public:
    bool allows(const DadosAtividade& dados) const override { 
        return dados.type() != "Acampamento"; 
    }
};

// Cadastrar atividade: Regra do cliente que recusa o tipo.
static void testAtvRegraDoCliente() {
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    prepararSecoes(chefe);
    Sessao sessaoChefe(chefe);
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    RegraSemAcampamento regra;
    atividades.add(regra);
    Atividade nova;
    DadosAtividade recusada("Acampamento", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "Escoteiro");
    verify(!atividades.registrar(recusada, nova, idChefe) && Repositorio::instance().atividades.empty(),
        "regra do cliente que recusa o tipo impede o cadastro");
    DadosAtividade aceita("Reuniao", Data(2026, 11, 15), "08:30", "Sede", "Reuniao", "Escoteiro");
    verify(atividades.registrar(aceita, nova, idChefe) && Repositorio::instance().atividades.size() == 1,
        "tipo aceito pela regra do cliente e cadastrado");
}

// ---------- Consultar atividade/evento ----------
struct CenarioConsulta {
    Usuario chefe{2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2)};
    Sessao sessaoChefe{chefe};
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    CenarioConsulta() { Repositorio::instance().clear(); }
    ~CenarioConsulta() { Repositorio::instance().clear(); }
};

// Fonte: Projeto da API Sprint 2 (1)(1), p. 12, Consultar atividade/evento.
// CT-01: tres atividades em outubro/2026 na secao do Chefe; uma Confirmada.
void testConsultaCT01FiltrarEVisualizarDetalhes() {
    CenarioConsulta cenario;
    DadosAtividade dados("Acampamento de Outubro", Data(2026, 10, 3), "08:00",
        "Parque Estadual", "Acampamento", "Escoteiro", "Acampamento da secao 2");
    // Situacoes preexistentes sao preparacao do cenario, nao novas funcionalidades.
    Atividade confirmada(10, dados, 2, "Confirmada");
    confirmada.enroll(7);
    Repositorio& repositorio = Repositorio::instance();
    repositorio.atividades.push_back(confirmada);
    repositorio.atividades.push_back(Atividade(11, DadosAtividade("Reuniao", Data(2026, 10, 10), "09:00", "Sede", "Reuniao", "Escoteiro"), 2, "Cancelada"));
    repositorio.atividades.push_back(Atividade(12, DadosAtividade("Trilha", Data(2026, 10, 17), "07:00", "Parque", "Passeio", "Escoteiro"), 2, "Realizada"));

    FiltroAtividade filtro(Data(2026, 10, 1), Data(2026, 10, 31), "Confirmada");
    Colecao<Atividade> lista = cenario.atividades.query(filtro, cenario.idChefe);
    verify(lista.size() == 1, "CT-01: apenas a atividade Confirmada e retornada");
    Iterador<Atividade> it = lista.iterator();
    verify(it.hasNext(), "CT-01: o Chefe pode iniciar a leitura pelo iterador");
    if (it.hasNext()) {
        const Atividade& a = it.next();
        verify(a.id() == 10 && a.sectionId() == 2 && a.status() == "Confirmada"
            && a.details().title() == dados.title() && a.details().date() == dados.date()
            && a.details().startTime() == dados.startTime() && a.details().place() == dados.place()
            && a.details().type() == dados.type() && a.details().branch() == dados.branch()
            && a.details().description() == dados.description()
            && a.members().size() == 1 && a.members()[0] == 7,
            "CT-01: iterador entrega a atividade correta com todos os detalhes");
    }
    verify(!it.hasNext(), "CT-01: nenhuma atividade Cancelada ou Realizada aparece na lista");
    verify(repositorio.atividades.size() == 3, "CT-01: consulta nao remove atividades cadastradas");
}

// CT-02, primeira alternativa: nenhuma atividade no periodo pedido.
void testConsultaCT02SemAtividadesNoPeriodo() {
    CenarioConsulta cenario;
    Repositorio::instance().atividades.push_back(Atividade(10,
        DadosAtividade("Reuniao", Data(2026, 9, 30), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada"));
    Colecao<Atividade> lista = cenario.atividades.query(
        FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), cenario.idChefe);
    verify(lista.size() == 0 && !lista.iterator().hasNext(),
        "CT-02a: periodo sem atividades retorna colecao vazia, sem erro");
}

// CT-02, segunda alternativa: so existem atividades de outra secao.
void testConsultaCT02AtividadesDeOutraSecao() {
    CenarioConsulta cenario;
    Repositorio::instance().atividades.push_back(Atividade(10,
        DadosAtividade("Reuniao da secao 3", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 3, "Confirmada"));
    Colecao<Atividade> lista = cenario.atividades.query(
        FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), cenario.idChefe);
    verify(lista.size() == 0 && !lista.iterator().hasNext(),
        "CT-02b: atividades de outra secao nao sao exibidas ao Chefe");
}

// ---------- Registrar presenca, ausencia ou atraso ----------

// CT-01: atividade 12 encerrada, nao cancelada, da secao do Chefe, integrantes 7 e 8.
static void testPresCT01RegistroValido() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);

    Colecao<Usuario> integrantes = presenca.listMembers(12, idChefe);
    verify(integrantes.size() == 2 && integrantes.at(0).id() == 7 && integrantes.at(1).id() == 8,
        "CT-01: listMembers apresenta os integrantes 7 e 8");
    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Atraso"));
    bool gravou = presenca.record(12, lista, idChefe);
    const vector<Participacao>& v = r.participacoes;
    verify(gravou && v.size() == 2, "CT-01: record retorna true e grava as duas participacoes");
    verify(v.size() == 2 && v[0].memberId() == 7 && v[0].status() == "Presente"
        && v[1].memberId() == 8 && v[1].status() == "Atraso"
        && v[0].activityId() == 12 && v[1].activityId() == 12,
        "CT-01: 7 Presente e 8 Atraso gravados na atividade 12");
    int minutos = -1;
    verify(presenca.recordingTime(12, minutos, idChefe) && minutos >= 0,
        "CT-01: recordingTime retorna true com os minutos desde o fim da atividade");
}

// CT-02: integrante 8 ja tem situacao registrada na atividade 12.
static void testPresCT02Duplicado() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);

    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Atraso"));
    presenca.record(12, lista, idChefe);
    bool dup = presenca.record(12, Participacao(8, "Ausente"), idChefe);
    verify(!dup, "CT-02: registrar de novo o integrante 8 retorna false");
    const vector<Participacao>& v = r.participacoes;
    bool mantida = false;
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].memberId() == 8 && v[i].activityId() == 12) mantida = (v[i].status() == "Atraso");
    verify(v.size() == 2 && mantida, "CT-02: a situacao anterior do integrante 8 permanece");
}

// Atividade que ainda nao terminou.
static void testPresAtividadeEmAndamento() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    Atividade futura(13, DadosAtividade("Reuniao", Data(2099, 1, 1), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    futura.enroll(7);
    futura.enroll(8);
    r.atividades.push_back(futura);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);

    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Presente"));
    verify(!presenca.record(13, lista, idChefe) && r.participacoes.empty(),
        "atividade em andamento: record retorna false e nada e gravado");
    int minutos = -1;
    verify(!presenca.recordingTime(13, minutos, idChefe), "atividade em andamento: recordingTime retorna false");
}

// Lote invalido (tudo ou nada).
static void testPresLoteInvalido() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(9, DadosUsuario("Caio", Data(2012, 7, 1), "UEL", "Jovem", 3)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);

    Colecao<Participacao> lote;
    lote.add(Participacao(7, "Presente"));
    lote.add(Participacao(8, "Faltou"));
    verify(!presenca.record(12, lote, idChefe) && r.participacoes.empty(),
        "lote invalido: nenhum item e gravado");
    Colecao<Participacao> comNaoInscrito;
    comNaoInscrito.add(Participacao(7, "Presente"));
    comNaoInscrito.add(Participacao(9, "Presente"));
    verify(!presenca.record(12, comNaoInscrito, idChefe) && r.participacoes.empty(),
        "lote com integrante nao inscrito: nenhum item e gravado");
}

// Chefe de outra secao.
static void testPresOutraSecao() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Usuario chefeOutro(4, DadosUsuario("Chefe 3", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(chefeOutro);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe), sessaoOutro(chefeOutro);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    int idOutro = presenca.add(sessaoOutro);

    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Presente"));
    verify(presenca.listMembers(12, idOutro).size() == 0, "outra secao: listMembers vazio");
    verify(!presenca.record(12, lista, idOutro) && r.participacoes.empty(),
        "outra secao: record retorna false e nada e gravado");
    presenca.record(12, lista, idChefe);
    int minutos = -1;
    verify(!presenca.recordingTime(12, minutos, idOutro), "outra secao: recordingTime retorna false");
}

// Atividade cancelada.
static void testPresCancelada() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    Atividade cancelada(14, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Cancelada");
    cancelada.enroll(7);
    cancelada.enroll(8);
    r.atividades.push_back(cancelada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);

    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Presente"));
    verify(!presenca.record(14, lista, idChefe) && r.participacoes.empty(),
        "atividade cancelada: record retorna false");
}

// Sessao de Jovem.
static void testPresSessaoJovem() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2));
    r.usuarios.push_back(jovem);
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    r.atividades.push_back(encerrada);
    Sessao sessaoJovem(jovem);
    ServicoPresenca presenca;
    int idJovem = presenca.add(sessaoJovem);

    verify(presenca.listMembers(12, idJovem).size() == 0
        && !presenca.record(12, Participacao(7, "Presente"), idJovem) && r.participacoes.empty(),
        "sessao de Jovem: listMembers vazio e record retorna false");
}

int runFunctionalTests() {
    falhas = 0;
    cout << "\n=== FUNCIONAIS: Cadastrar usuario ===\n";
    testCT01CadastroValido();
    testCT02CadastroDuplicado();
    cout << "\n=== FUNCIONAIS: Cadastrar atividade/evento ===\n";
    testAtvCT01CadastroValido();
    testAtvCT02SemTitulo();
    testAtvHorarioInvalido();
    testAtvSessaoNaoChefe();
    testAtvRegraDoCliente();
    cout << "\n=== FUNCIONAIS: Consultar atividade/evento ===\n";
    testConsultaCT01FiltrarEVisualizarDetalhes();
    testConsultaCT02SemAtividadesNoPeriodo();
    testConsultaCT02AtividadesDeOutraSecao();
    cout << "\n=== FUNCIONAIS: Registrar presenca,ausencia ou atraso ===\n";
    testPresCT01RegistroValido();
    testPresCT02Duplicado();
    testPresAtividadeEmAndamento();
    testPresLoteInvalido();
    testPresOutraSecao();
    testPresCancelada();
    testPresSessaoJovem();
    Repositorio::instance().clear();
    Repositorio::instance().clear();
    return falhas;
}
