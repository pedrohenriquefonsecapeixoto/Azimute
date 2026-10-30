#include "funcional_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
#include "../../src/moduloConcre/ServicoAtividades.h"
#include "../../src/moduloConcre/Repositorio.h"
#include <string>
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
    Repositorio::instance().clear();
    return falhas;
}
