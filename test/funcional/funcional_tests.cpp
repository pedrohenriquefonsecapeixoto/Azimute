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
#undef NDEBUG
#include <cassert>
using namespace std;

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
    assert(ok && "CT-01: registrar deve retornar true");
    assert(nova.id() >= 100 && "CT-01: usuario recebe id novo (Repositorio gera ids a partir de 100)");
    assert(nova.role() == "Jovem" && "CT-01: papel Jovem");
    assert(nova.sectionId() == 2 && "CT-01: secao 2");
    assert(repositorio.usuarios.size() == 1 && "CT-01: exatamente um usuario foi gravado");
    const Usuario* salvo = repositorio.findUser(nova.id());
    assert(salvo != nullptr && "CT-01: o usuario gravado e encontrado pelo id devolvido");
    assert(igual(nova, *salvo) && "CT-01: o usuario de saida e identico ao gravado");
    assert(salvo->details().name() == "Ana Souza" && "CT-01: nome gravado");
    assert(salvo->details().birth() == Data(2012, 5, 20) && "CT-01: nascimento gravado");
    assert(salvo->details().bond() == "UEL" && "CT-01: vinculo gravado");
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
    assert(preparado && "CT-02 (preparo): o primeiro cadastro da Ana deve funcionar");
    Usuario original(77, DadosUsuario("Usuario anterior", Data(1990, 3, 10), "Outro vinculo", "Chefe Escotista", 3));
    Usuario nova = original;
    bool ok = usuarios.registrar(dados, nova, idDirigenteUsuarios);
    assert(!ok && "CT-02: registrar com mesmo nome e nascimento deve retornar false");
    assert(repositorio.usuarios.size() == 1 && "CT-02: nenhum usuario novo e criado");
    assert(igual(repositorio.usuarios[0], cadastrada) && "CT-02: o cadastro existente nao muda");
    assert(igual(nova, original) && "CT-02: o usuario de saida nao muda");
}

// Cadastrar usuario, alternativa: campos invalidos impedem a criacao.
static void testCadastroCamposInvalidos() {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    Usuario dirigente(1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoDirigente(dirigente);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    const DadosUsuario invalidos[] = {
        DadosUsuario("", Data(2012, 5, 20), "UEL", "Jovem", 2),            // nome vazio
        DadosUsuario("Ana Souza", Data(2012, 2, 30), "UEL", "Jovem", 2),   // nascimento inexistente
        DadosUsuario("Ana Souza", Data(2012, 5, 20), "", "Jovem", 2),      // vinculo vazio
        DadosUsuario("Ana Souza", Data(2012, 5, 20), "UEL", "Visitante", 2) // papel nao permitido
    };
    for (const DadosUsuario& dados : invalidos) {
        Usuario nova;
        bool ok = usuarios.registrar(dados, nova, idDirigenteUsuarios);
        assert(!ok && "campo invalido (nome, nascimento, vinculo ou papel): registrar retorna false");
        assert(repositorio.usuarios.empty() && "campo invalido: nenhum usuario e criado");
    }
}

// Cadastrar usuario, passo 3: so o Dirigente cadastra.
static void testCadastroSessaoNaoDirigente() {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Sessao sessaoChefe(chefe);
    ServicoUsuarios usuarios;
    int idChefe = usuarios.add(sessaoChefe);
    Usuario nova;
    bool ok = usuarios.registrar(DadosUsuario("Ana Souza", Data(2012, 5, 20), "UEL", "Jovem", 2), nova, idChefe);
    assert(!ok && "sessao de Chefe: registrar retorna false");
    bool semSessao = usuarios.registrar(DadosUsuario("Ana Souza", Data(2012, 5, 20), "UEL", "Jovem", 2), nova, 999);
    assert(!semSessao && "sessao inexistente: registrar retorna false");
    assert(repositorio.usuarios.empty() && "sem Dirigente em sessao: nenhum usuario e criado");
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
    assert(ok && "CT-01: registrar deve retornar true");
    assert(nova.id() >= 100 && "CT-01: atividade recebe id proprio");
    assert(nova.status() == "Confirmada" && "CT-01: situacao Confirmada, sem pedido ao Dirigente");
    assert(nova.sectionId() == 2 && "CT-01: secao do Chefe");
    Repositorio& repositorio = Repositorio::instance();
    assert(repositorio.atividades.size() == 1 && "CT-01: exatamente uma atividade foi gravada");
    const Atividade* salva = repositorio.findActivity(nova.id());
    assert(salva != nullptr && "CT-01: a atividade gravada e encontrada pelo id devolvido");
    assert(igualAtv(nova, *salva) && "CT-01: a atividade de saida e identica a gravada");
    assert(salva->details().title() == "Acampamento de Primavera" && "CT-01: titulo gravado");
    assert(salva->details().startTime() == "08:30" && "CT-01: horario gravado");
    assert(nova.members().size() == 2 && "CT-01: so os 2 Jovens da secao do Chefe sao inscritos");
    assert(nova.members()[0] == 10 && nova.members()[1] == 11 && "CT-01: inscritos 10 e 11 (o Jovem 12 e de outra secao)");
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
    assert(!ok && "CT-02: registrar sem titulo deve retornar false");
    assert(Repositorio::instance().atividades.empty() && "CT-02: nenhuma atividade e criada");
    assert(igualAtv(nova, original) && "CT-02: a atividade de saida nao muda");
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
        bool ok = atividades.registrar(dados, nova, idChefe);
        assert(!ok && "horario fora do formato HH:MM (25:00 ou 8:00): registrar retorna false");
        assert(Repositorio::instance().atividades.empty() && "horario invalido: nenhuma atividade e criada");
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
    bool ok = atividades.registrar(dados, nova, idJovem);
    assert(!ok && "sessao de Jovem: registrar retorna false");
    assert(Repositorio::instance().atividades.empty() && "sessao de Jovem: nenhuma atividade e criada");
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
    bool okRecusada = atividades.registrar(recusada, nova, idChefe);
    assert(!okRecusada && "regra do cliente que recusa o tipo: registrar retorna false");
    assert(Repositorio::instance().atividades.empty() && "regra do cliente: nenhuma atividade e criada");
    DadosAtividade aceita("Reuniao", Data(2026, 11, 15), "08:30", "Sede", "Reuniao", "Escoteiro");
    bool okAceita = atividades.registrar(aceita, nova, idChefe);
    assert(okAceita && "tipo aceito pela regra do cliente: registrar retorna true");
    assert(Repositorio::instance().atividades.size() == 1 && "tipo aceito: a atividade e gravada");
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


// CT-01: tres atividades em outubro/2026 na secao do Chefe; uma Confirmada.
static void testConsultaCT01FiltrarEVisualizarDetalhes() {
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
    assert(lista.size() == 1 && "CT-01: a Colecao tem tamanho 1 (so a Confirmada)");
    Iterador<Atividade> it = lista.iterator();
    assert(it.hasNext() && "CT-01: o iterador tem a atividade");
    const Atividade& a = it.next();   // avanca o iterador FORA do assert
    assert(a.id() == 10 && "CT-01: e a atividade Confirmada (id 10)");
    assert(a.sectionId() == 2 && a.status() == "Confirmada" && "CT-01: secao 2 e situacao Confirmada");
    assert(a.details().title() == dados.title() && "CT-01: titulo");
    assert(a.details().date() == dados.date() && a.details().startTime() == dados.startTime() && "CT-01: data e horario");
    assert(a.details().place() == dados.place() && "CT-01: local");
    assert(a.details().type() == dados.type() && a.details().branch() == dados.branch() && "CT-01: tipo e ramo");
    assert(a.details().description() == dados.description() && "CT-01: descricao");
    assert(a.members().size() == 1 && a.members()[0] == 7 && "CT-01: integrante inscrito");
    assert(!it.hasNext() && "CT-01: Cancelada e Realizada nao aparecem na lista");
    assert(repositorio.atividades.size() == 3 && "CT-01: a consulta nao remove atividades cadastradas");
}

// CT-02, primeira alternativa: nenhuma atividade no periodo pedido.
static void testConsultaCT02SemAtividadesNoPeriodo() {
    CenarioConsulta cenario;
    Repositorio::instance().atividades.push_back(Atividade(10,
        DadosAtividade("Reuniao", Data(2026, 9, 30), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada"));
    Colecao<Atividade> lista = cenario.atividades.query(
        FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), cenario.idChefe);
    assert(lista.size() == 0 && "CT-02a: periodo sem atividades devolve Colecao vazia (tamanho 0)");
    assert(!lista.iterator().hasNext() && "CT-02a: o iterador nao tem itens");
}

// CT-02, segunda alternativa: so existem atividades de outra secao.
static void testConsultaCT02AtividadesDeOutraSecao() {
    CenarioConsulta cenario;
    Repositorio::instance().atividades.push_back(Atividade(10,
        DadosAtividade("Reuniao da secao 3", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 3, "Confirmada"));
    Colecao<Atividade> lista = cenario.atividades.query(
        FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), cenario.idChefe);
    assert(lista.size() == 0 && "CT-02b: atividades de outra secao nao sao mostradas ao Chefe");
    assert(!lista.iterator().hasNext() && "CT-02b: o iterador nao tem itens");
}

// Consultar atividade: so o Chefe consulta.
static void testConsultaSessaoNaoChefe() {
    CenarioConsulta cenario;
    Repositorio::instance().atividades.push_back(Atividade(10,
        DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada"));
    Usuario jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2));
    Sessao sessaoJovem(jovem);
    int idJovem = cenario.atividades.add(sessaoJovem);
    FiltroAtividade outubro(Data(2026, 10, 1), Data(2026, 10, 31));
    assert(cenario.atividades.query(outubro, idJovem).size() == 0 && "sessao de Jovem: Colecao vazia");
    assert(cenario.atividades.query(outubro, 999).size() == 0 && "sessao inexistente: Colecao vazia");
    assert(cenario.atividades.query(outubro, cenario.idChefe).size() == 1 && "controle: o Chefe da secao ve a atividade");
}


static void executar(const char* nome, void (*teste)()) {
    teste();
    cout << "[OK] " << nome << '\n';
}

void runFunctionalTests() {
    cout << "\n=== FUNCIONAIS: Cadastrar usuario ===\n";
    executar("CT-01 cadastro valido", testCT01CadastroValido);
    executar("CT-02 cadastro duplicado", testCT02CadastroDuplicado);
    executar("campos invalidos", testCadastroCamposInvalidos);
    executar("sessao que nao e de Dirigente", testCadastroSessaoNaoDirigente);
    cout << "\n=== FUNCIONAIS: Cadastrar atividade/evento ===\n";
    executar("CT-01 cadastro valido", testAtvCT01CadastroValido);
    executar("CT-02 sem titulo", testAtvCT02SemTitulo);
    executar("horario invalido", testAtvHorarioInvalido);
    executar("sessao que nao e de Chefe", testAtvSessaoNaoChefe);
    executar("regra do cliente", testAtvRegraDoCliente);
    cout << "\n=== FUNCIONAIS: Consultar atividade/evento ===\n";
    executar("CT-01 filtrar e visualizar detalhes", testConsultaCT01FiltrarEVisualizarDetalhes);
    executar("CT-02a sem atividades no periodo", testConsultaCT02SemAtividadesNoPeriodo);
    executar("CT-02b atividades de outra secao", testConsultaCT02AtividadesDeOutraSecao);
    executar("sessao que nao e de Chefe", testConsultaSessaoNaoChefe);
}