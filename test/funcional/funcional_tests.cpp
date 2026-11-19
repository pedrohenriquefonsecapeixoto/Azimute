#include "funcional_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
#include "../../src/moduloConcre/ServicoAtividades.h"
#include "../../src/moduloConcre/Repositorio.h"
#include "../../src/moduloConcre/ServicoPresenca.h"
#include <string>
#include <new>
#include <ctime>
#include <vector>
#include "../../include/Colecao.h"
#undef NDEBUG
#include <cassert>
using namespace std;


/**
 * @brief Compara dois usuarios campo a campo (id, nome, nascimento, vinculo, papel e secao).
 * @param a Primeiro usuario a comparar.
 * @param b Segundo usuario a comparar.
 * @return true se todos os campos comparados forem iguais; false caso contrario.
 */
static bool igual(const Usuario& a, const Usuario& b) {
    return a.id() == b.id() && a.details().name() == b.details().name()
        && a.details().birth() == b.details().birth() && a.details().bond() == b.details().bond()
        && a.role() == b.role() && a.sectionId() == b.sectionId();
}

/**
 * @brief CT-01 de Cadastrar usuario: cadastro valido feito pelo Dirigente.
 * Verifica que registrar retorna true, gera id novo, grava exatamente um usuario e que o usuario de saida e identico ao gravado no Repositorio.
 * @param Nenhum.
 * @return Nada (void).
 */
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

/**
 * @brief CT-02 de Cadastrar usuario: recusa cadastro duplicado (mesmo nome e mesma data de nascimento).
 * Verifica que registrar retorna false, nenhum usuario novo e criado, o cadastro existente nao muda e o usuario de saida permanece inalterado.
 * @param Nenhum.
 * @return Nada (void).
 */
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

/**
 * @brief Fluxo alternativo de Cadastrar usuario: campos invalidos impedem a criacao.
 * Testa nome vazio, nascimento inexistente (30/02), vinculo vazio e papel nao permitido; em todos registrar retorna false e o Repositorio continua vazio.
 * @param Nenhum.
 * @return Nada (void).
 */
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

/**
 * @brief Passo 3 de Cadastrar usuario: somente a sessao de Dirigente pode cadastrar.
 * Verifica que sessao de Chefe e sessao inexistente recebem false e que nenhum usuario e criado.
 * @param Nenhum.
 * @return Nada (void).
 */
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

/**
 * @brief Compara duas atividades campo a campo (usada nos testes de cadastro de atividade).
 *
 * @param a Primeira atividade.
 * @param b Segunda atividade.
 * @return true se id, dados (título, data, horário, local, tipo e ramo), seção, situação e integrantes forem iguais; false caso contrário.
 */
static bool igualAtv(const Atividade& a, const Atividade& b) {
    return a.id() == b.id() && a.details().title() == b.details().title()
        && a.details().date() == b.details().date() && a.details().startTime() == b.details().startTime()
        && a.details().place() == b.details().place() && a.details().type() == b.details().type()
        && a.details().branch() == b.details().branch() && a.sectionId() == b.sectionId()
        && a.status() == b.status() && a.members() == b.members();
}

/**
 * @brief Prepara o Repositório para os testes de cadastro de atividade.
 *
 * Limpa o Repositório e grava o Chefe recebido (seção 2), os Jovens 10 e 11 (seção 2)
 * e o Jovem 12 (seção 3).
 *
 * @param chefe Chefe Escotista da seção 2 que será gravado no Repositório.
 */
static void prepararSecoes(const Usuario& chefe) {
    Repositorio& repositorio = Repositorio::instance();
    repositorio.clear();
    repositorio.usuarios.push_back(chefe);
    repositorio.usuarios.push_back(Usuario(10, DadosUsuario("Jovem A", Data(2012, 1, 1), "UEL", "Jovem", 2)));
    repositorio.usuarios.push_back(Usuario(11, DadosUsuario("Jovem B", Data(2012, 2, 2), "UEL", "Jovem", 2)));
    repositorio.usuarios.push_back(Usuario(12, DadosUsuario("Jovem C", Data(2012, 3, 3), "UEL", "Jovem", 3)));
}

/**
 * @brief Teste CT-01 do cadastro de atividade: Chefe da seção 2 com dados completos.
 *
 * Verifica que registrar retorna true, que a atividade recebe id próprio e situação "Confirmada",
 * que fica gravada no Repositório e que só os Jovens da seção do Chefe (10 e 11) são inscritos.
 */
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

/**
 * @brief Teste CT-02 do cadastro de atividade: campo obrigatório vazio (sem título).
 *
 * Verifica que registrar retorna false, que nenhuma atividade é criada e que a atividade de saída não muda.
 */
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

/**
 * @brief Teste do cadastro de atividade com horário inválido ("25:00" e "8:00", fora do formato HH:MM).
 *
 * Verifica que registrar retorna false e que nenhuma atividade é criada.
 */
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

/**
 * @brief Teste do cadastro de atividade com sessão que não é de Chefe (sessão de Jovem).
 *
 * Verifica que registrar retorna false e que nenhuma atividade é criada.
 */
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

/**
 * @brief Regra do cliente usada no teste: recusa atividades do tipo "Acampamento".
 * Atributos: nenhum.
 */
class RegraSemAcampamento : public Regra<DadosAtividade> {
public:
    /**
     * @brief Decide se os dados de uma atividade são aceitos pela regra.
     *
     * @param dados Dados da atividade que se deseja cadastrar.
     * @return false se o tipo for "Acampamento"; true para qualquer outro tipo.
     */
    bool allows(const DadosAtividade& dados) const override { 
        return dados.type() != "Acampamento"; 
    }
};

/**
 * @brief Teste do cadastro de atividade com uma Regra do cliente que recusa o tipo "Acampamento".
 *
 * Verifica que registrar retorna false para o tipo recusado (nada é criado) e true para um tipo aceito.
 */
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

// ---------- Registrar presenca, ausencia ou atraso ----------

/**
 * @brief CT-01 de Registrar presenca, ausencia ou atraso: registro valido do Chefe da secao.
 * Atividade 12 encerrada e nao cancelada, com integrantes 7 e 8. Verifica listMembers, record e recordingTime.
 * @param Nenhum.
 * @return Nada (void);
 */
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
    assert(integrantes.size() == 2 && "CT-01: listMembers apresenta 2 integrantes");
    assert(integrantes.at(0).id() == 7 && integrantes.at(1).id() == 8 && "CT-01: integrantes 7 e 8");
    Colecao<Participacao> lista;
    lista.add(Participacao(7, "Presente"));
    lista.add(Participacao(8, "Atraso"));
    bool gravou = presenca.record(12, lista, idChefe);
    const vector<Participacao>& v = r.participacoes;
    assert(gravou && "CT-01: record deve retornar true");
    assert(v.size() == 2 && "CT-01: as duas participacoes foram gravadas");
    assert(v[0].memberId() == 7 && v[0].status() == "Presente" && "CT-01: integrante 7 Presente");
    assert(v[1].memberId() == 8 && v[1].status() == "Atraso" && "CT-01: integrante 8 Atraso");
    assert(v[0].activityId() == 12 && v[1].activityId() == 12 && "CT-01: participacoes vinculadas a atividade 12");
    int minutos = -1;
    bool medido = presenca.recordingTime(12, minutos, idChefe);
    assert(medido && "CT-01: recordingTime deve retornar true");
    assert(minutos >= 0 && "CT-01: minutos desde o fim da atividade (nunca negativo)");
}

/**
 * @brief CT-02 de Registrar presenca, ausencia ou atraso: recusa novo registro para integrante que ja tem situacao.
 * Apos o primeiro registro, record do integrante 8 retorna false, nada novo e gravado e a situacao anterior permanece.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    bool preparado = presenca.record(12, lista, idChefe);
    assert(preparado && "CT-02 (preparo): o primeiro registro deve funcionar");
    bool dup = presenca.record(12, Participacao(8, "Ausente"), idChefe);
    assert(!dup && "CT-02: registrar de novo o integrante 8 deve retornar false");
    const vector<Participacao>& v = r.participacoes;
    assert(v.size() == 2 && "CT-02: nenhuma participacao nova e gravada");
    bool mantida = false;
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].memberId() == 8 && v[i].activityId() == 12) 
            mantida = (v[i].status() == "Atraso");
    assert(mantida && "CT-02: a situacao anterior do integrante 8 (Atraso) permanece");
}

/**
 * @brief Fluxo alternativo: atividade que ainda nao terminou.
 * Verifica que record e recordingTime retornam false, nada e gravado e o valor de minutos nao e alterado.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    bool gravou = presenca.record(13, lista, idChefe);
    assert(!gravou && "atividade em andamento: record retorna false");
    assert(r.participacoes.empty() && "atividade em andamento: nada e gravado");
    int minutos = -1;
    bool medido = presenca.recordingTime(13, minutos, idChefe);
    assert(!medido && "atividade em andamento: recordingTime retorna false");
    assert(minutos == -1 && "atividade em andamento: minutos nao e alterado");
}

/**
 * @brief Fluxo alternativo: lote invalido e rejeitado por inteiro.
 * Testa lote com situacao invalida e lote com integrante nao inscrito.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    bool okSituacao = presenca.record(12, lote, idChefe);
    assert(!okSituacao && "lote com situacao invalida (Faltou): record retorna false");
    assert(r.participacoes.empty() && "lote com situacao invalida: nenhum item e gravado (tudo ou nada)");
    Colecao<Participacao> comNaoInscrito;
    comNaoInscrito.add(Participacao(7, "Presente"));
    comNaoInscrito.add(Participacao(9, "Presente"));
    bool okInscrito = presenca.record(12, comNaoInscrito, idChefe);
    assert(!okInscrito && "lote com integrante nao inscrito: record retorna false");
    assert(r.participacoes.empty() && "lote com integrante nao inscrito: nenhum item e gravado");
}

/**
 * @brief Fluxo alternativo: Chefe de outra secao nao registra nem consulta a atividade.
 * Verifica que listMembers vem vazio, record e recordingTime retornam false e nada e gravado; como controle, o Chefe da secao consegue registrar a mesma lista.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    assert(presenca.listMembers(12, idOutro).size() == 0 && "outra secao: listMembers vazio");
    bool okOutro = presenca.record(12, lista, idOutro);
    assert(!okOutro && "outra secao: record retorna false");
    assert(r.participacoes.empty() && "outra secao: nada e gravado");
    bool okChefe = presenca.record(12, lista, idChefe);
    assert(okChefe && "controle: o Chefe da secao consegue registrar a mesma lista");
    int minutos = -1;
    bool medido = presenca.recordingTime(12, minutos, idOutro);
    assert(!medido && "outra secao: recordingTime retorna false mesmo com registros");
    assert(minutos == -1 && "outra secao: minutos nao e alterado");
}

/**
 * @brief Fluxo alternativo: atividade cancelada nao aceita registro de presenca.
 * Verifica que record retorna false e nenhuma participacao e gravada.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    bool gravou = presenca.record(14, lista, idChefe);
    assert(!gravou && "atividade cancelada: record retorna false");
    assert(r.participacoes.empty() && "atividade cancelada: nada e gravado");
}

/**
 * @brief Fluxo alternativo: sessao de Jovem nao registra presenca.
 * Verifica que listMembers vem vazio, record retorna false e nada e gravado.
 * @param Nenhum.
 * @return Nada (void).
 */
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
    assert(presenca.listMembers(12, idJovem).size() == 0 && "sessao de Jovem: listMembers vazio");
    bool gravou = presenca.record(12, Participacao(7, "Presente"), idJovem);
    assert(!gravou && "sessao de Jovem: record retorna false");
    assert(r.participacoes.empty() && "sessao de Jovem: nada e gravado");
}

// ---------- Consultar historico de participacao ----------

/**
 * @brief Compara dois números reais, aceitando uma diferença muito pequena (menor que 0,0001).
 *
 * @param a Primeiro número.
 * @param b Segundo número.
 * @return true se a diferença entre os dois for menor que 0,0001; false caso contrário.
 */
static bool pertoDe(double a, double b) {
    double d = a - b;
    return (d < 0 ? -d : d) < 0.0001;
}

/**
 * @brief Cenário de outubro/2026 usado nos testes do histórico de participação.
 * Atributos: dirigente, carlos, dora, ana, beto e caio (usuários); sessaoDirigente, sessaoCarlos, sessaoDora,
 * sessaoAna e sessaoBeto (sessões deles); presenca (o ServicoPresenca testado); idDirigente, idCarlos, idDora,
 * idAna e idBeto (identificadores das sessões no serviço).
 *
 * Usuários: Dirigente 1, Chefes 2 (seção 2) e 4 (seção 3), Jovens Ana 7 e Beto 8 (seção 2) e Caio 9 (seção 3).
 * Atividades: 1 a 4 em outubro (seção 2), 5 em setembro, 6 cancelada e 7 da seção 3.
 * Ana tem Presente, Atraso, Ausente e Presente em outubro (3 comparecimentos em 4), mais um registro
 * fora do período (atividade 5) e outro em atividade cancelada (atividade 6).
 */
struct CenarioHistorico {
    Usuario dirigente{1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0)};
    Usuario carlos{2, DadosUsuario("Carlos", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2)};
    Usuario dora{4, DadosUsuario("Dora", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3)};
    Usuario ana{7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)};
    Usuario beto{8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)};
    Usuario caio{9, DadosUsuario("Caio", Data(2012, 7, 1), "UEL", "Jovem", 3)};
    Sessao sessaoDirigente{dirigente};
    Sessao sessaoCarlos{carlos};
    Sessao sessaoDora{dora};
    Sessao sessaoAna{ana};
    Sessao sessaoBeto{beto};
    ServicoPresenca presenca;
    int idDirigente, idCarlos, idDora, idAna, idBeto;

    /**
     * @brief Grava uma atividade realizada (ou com a situação informada) no Repositório, em 2026.
     *
     * @param idAtividade Identificador da atividade.
     * @param dia Dia da atividade.
     * @param mes Mês da atividade.
     * @param idSecao Identificador da seção da atividade.
     * @param situacao Situação da atividade (Confirmada, Realizada ou Cancelada).
     */
    static void guardar(int idAtividade, int dia, int mes, int idSecao, const string& situacao) {
        Atividade atividade(idAtividade, DadosAtividade("Reuniao", Data(2026, mes, dia), "08:00", "Sede", "Reuniao", "Escoteiro"),
                            idSecao, situacao);
        Repositorio::instance().atividades.push_back(atividade);
    }
    /**
     * @brief Grava no Repositório a participação de um jovem em uma atividade.
     *
     * @param idAtividade Identificador da atividade.
     * @param idJovem Identificador do jovem.
     * @param situacao Situação da participação (Presente, Ausente ou Atraso).
     */
    static void registrar(int idAtividade, int idJovem, const string& situacao) {
        Participacao participacao(idJovem, situacao);
        participacao.attach(idAtividade, time(nullptr));
        Repositorio::instance().participacoes.push_back(participacao);
    }

    /**
     * @brief Monta o cenário: limpa o Repositório, grava usuários, atividades e participações e registra as sessões no serviço.
     */
    CenarioHistorico() {
        Repositorio& r = Repositorio::instance();
        r.clear();
        r.usuarios.push_back(dirigente);
        r.usuarios.push_back(carlos);
        r.usuarios.push_back(dora);
        r.usuarios.push_back(ana);
        r.usuarios.push_back(beto);
        r.usuarios.push_back(caio);
        guardar(1, 3, 10, 2, "Realizada");
        guardar(2, 10, 10, 2, "Realizada");
        guardar(3, 17, 10, 2, "Realizada");
        guardar(4, 24, 10, 2, "Realizada");
        guardar(5, 12, 9, 2, "Realizada");
        guardar(6, 31, 10, 2, "Cancelada");
        guardar(7, 5, 10, 3, "Realizada");
        registrar(1, 7, "Presente");
        registrar(2, 7, "Atraso");
        registrar(3, 7, "Ausente");
        registrar(4, 7, "Presente");
        registrar(5, 7, "Presente");
        registrar(6, 7, "Presente");
        registrar(1, 8, "Presente");
        registrar(2, 8, "Ausente");
        registrar(7, 9, "Presente");
        idDirigente = presenca.add(sessaoDirigente);
        idCarlos = presenca.add(sessaoCarlos);
        idDora = presenca.add(sessaoDora);
        idAna = presenca.add(sessaoAna);
        idBeto = presenca.add(sessaoBeto);
    }
    /**
     * @brief Cria um filtro de histórico para o período de 1 a 31 de outubro de 2026.
     *
     * @param idIntegrante Identificador do jovem consultado; 0 significa o próprio usuário da sessão (ou a seção, se idSecao for diferente de 0).
     * @param idSecao Identificador da seção consultada; 0 significa nenhuma seção.
     * @return Filtro do histórico para outubro de 2026.
     */
    FiltroHistorico outubro(int idIntegrante = 0, int idSecao = 0) const {
        return FiltroHistorico(Data(2026, 10, 1), Data(2026, 10, 31), idIntegrante, idSecao);
    }
};

/**
 * @brief Teste CT-01 do histórico: Ana (id 7), com 3 comparecimentos em 4 atividades de outubro/2026.
 *
 * Verifica que query traz só os 4 registros da Ana e que attendancePercentage retorna true com 75,0.
 * Informar o próprio id da Ana no filtro traz o mesmo histórico.
 */
static void testHistCT01Ana() {
    CenarioHistorico c;
    Colecao<Participacao> historico = c.presenca.query(c.outubro(), c.idAna);
    assert(historico.size() == 4 && "CT-01: query traz os 4 registros de outubro");
    for (Iterador<Participacao> i = historico.iterator(); i.hasNext(); ) {
        const Participacao& p = i.next();   // avanca o iterador FORA do assert
        assert(p.memberId() == 7 && "CT-01: so registros da Ana");
    }
    double percentual = 0;
    bool ok = c.presenca.attendancePercentage(c.outubro(), percentual, c.idAna);
    assert(ok && "CT-01: attendancePercentage retorna true");
    assert(pertoDe(percentual, 75.0) && "CT-01: 3 comparecimentos (Presente + Atraso) em 4 = 75,0");
    Colecao<Participacao> explicito = c.presenca.query(c.outubro(7), c.idAna);
    assert(explicito.size() == 4 && "CT-01: informar o proprio id da Ana traz o mesmo historico");
}

/**
 * @brief Teste CT-02 do histórico: Ana consultando o idIntegrante 8 (outro jovem).
 *
 * Verifica que query devolve Colecao vazia e attendancePercentage retorna false,
 * sem alterar o percentual, ou seja, sem revelar dado de outro integrante.
 */
static void testHistCT02AnaNaoVeOutro() {
    CenarioHistorico c;
    double percentual = -1;
    assert(c.presenca.query(c.outubro(8), c.idAna).size() == 0 && "CT-02: Colecao vazia para outro integrante");
    bool ok = c.presenca.attendancePercentage(c.outubro(8), percentual, c.idAna);
    assert(!ok && "CT-02: attendancePercentage retorna false");
    assert(pertoDe(percentual, -1) && "CT-02: percentual nao e alterado (nada de outro integrante e revelado)");
}

/**
 * @brief Teste do histórico: Dirigente consulta a seção 2 inteira.
 *
 * Verifica que recebe os 6 registros de outubro (Ana e Beto) e o percentual da seção (4 de 6),
 * e que também consegue consultar um Jovem de qualquer seção.
 */
static void testHistDirigenteSecao() {
    CenarioHistorico c;
    Colecao<Participacao> historico = c.presenca.query(c.outubro(0, 2), c.idDirigente);
    assert(historico.size() == 6 && "Dirigente: secao 2 devolve os 6 registros de outubro (Ana e Beto)");
    double percentual = 0;
    bool ok = c.presenca.attendancePercentage(c.outubro(0, 2), percentual, c.idDirigente);
    assert(ok && "Dirigente: attendancePercentage da secao retorna true");
    assert(pertoDe(percentual, 400.0 / 6.0) && "Dirigente: percentual da secao 2 inteira (4 de 6)");
    assert(c.presenca.query(c.outubro(9), c.idDirigente).size() == 1 && "Dirigente: consulta Jovem de qualquer secao");
}

/**
 * @brief Teste do histórico: Chefe consultando Jovem ou seção de outra seção.
 *
 * Verifica que recebe Colecao vazia e false, sem alterar o percentual,
 * e que na própria seção a consulta funciona normalmente (6 registros e 75,0 para a Ana).
 */
static void testHistChefeOutraSecao() {
    CenarioHistorico c;
    double percentual = -1;
    assert(c.presenca.query(c.outubro(9), c.idCarlos).size() == 0 && "Chefe: Jovem de outra secao, Colecao vazia");
    bool okJovem = c.presenca.attendancePercentage(c.outubro(9), percentual, c.idCarlos);
    assert(!okJovem && "Chefe: Jovem de outra secao, percentual false");
    assert(c.presenca.query(c.outubro(0, 3), c.idCarlos).size() == 0 && "Chefe: outra secao, Colecao vazia");
    bool okSecao = c.presenca.attendancePercentage(c.outubro(0, 3), percentual, c.idCarlos);
    assert(!okSecao && "Chefe: outra secao, percentual false");
    assert(pertoDe(percentual, -1) && "Chefe: o percentual nao e alterado nas recusas");
    assert(c.presenca.query(c.outubro(0, 2), c.idCarlos).size() == 6 && "Chefe da propria secao: consulta a secao (6 registros)");
    bool okProprio = c.presenca.attendancePercentage(c.outubro(7), percentual, c.idCarlos);
    assert(okProprio && "Chefe da propria secao: consulta a Ana, percentual true");
    assert(pertoDe(percentual, 75.0) && "Chefe da propria secao: percentual da Ana 75,0");
}

/**
 * @brief Teste do histórico: alvo que não é Jovem (Chefe ou Dirigente) é recusado.
 *
 * Verifica que Dirigente e Chefe recebem Colecao vazia e false ao consultar um Chefe ou um Dirigente.
 */
static void testHistAlvoNaoJovem() {
    CenarioHistorico c;
    double percentual = -1;
    assert(c.presenca.query(c.outubro(2), c.idDirigente).size() == 0 && "alvo Chefe: Dirigente recebe Colecao vazia");
    bool okChefe = c.presenca.attendancePercentage(c.outubro(2), percentual, c.idDirigente);
    assert(!okChefe && "alvo Chefe: Dirigente recebe false");
    assert(c.presenca.query(c.outubro(1), c.idDirigente).size() == 0 && "alvo Dirigente: Colecao vazia");
    bool okDirigente = c.presenca.attendancePercentage(c.outubro(1), percentual, c.idDirigente);
    assert(!okDirigente && "alvo Dirigente: percentual false");
    assert(c.presenca.query(c.outubro(2), c.idCarlos).size() == 0 && "alvo Chefe: Chefe recebe Colecao vazia");
    bool okChefeChefe = c.presenca.attendancePercentage(c.outubro(2), percentual, c.idCarlos);
    assert(!okChefeChefe && "alvo Chefe: Chefe recebe false");
}

/**
 * @brief Teste do histórico: período sem registros (novembro/2026).
 *
 * Verifica que query devolve Colecao vazia e attendancePercentage retorna false, sem alterar o percentual.
 */
static void testHistSemRegistros() {
    CenarioHistorico c;
    double percentual = -1;
    FiltroHistorico novembro(Data(2026, 11, 1), Data(2026, 11, 30));
    assert(c.presenca.query(novembro, c.idAna).size() == 0 && "sem registros no periodo: Colecao vazia");
    bool ok = c.presenca.attendancePercentage(novembro, percentual, c.idAna);
    assert(!ok && "sem registros no periodo: percentual false");
    assert(pertoDe(percentual, -1) && "sem registros no periodo: percentual nao e alterado");
}

/**
 * @brief Teste do histórico: atividade cancelada não entra na contagem.
 *
 * Verifica que, no dia da atividade cancelada (31/10/2026), query vem vazia e attendancePercentage retorna false,
 * e que o histórico de outubro da Ana continua com 4 registros.
 */
static void testHistCancelada() {
    CenarioHistorico c;
    double percentual = -1;
    FiltroHistorico dia31(Data(2026, 10, 31), Data(2026, 10, 31));
    assert(c.presenca.query(dia31, c.idAna).size() == 0 && "atividade cancelada nao entra na consulta");
    bool ok = c.presenca.attendancePercentage(dia31, percentual, c.idAna);
    assert(!ok && "atividade cancelada nao entra na contagem (percentual false)");
    assert(c.presenca.query(c.outubro(), c.idAna).size() == 4 && "atividade cancelada nao altera o historico de outubro");
}

/**
 * @brief Teste do histórico: sessão inexistente ou removida.
 *
 * Verifica que query devolve Colecao vazia e attendancePercentage retorna false,
 * e que remover a sessão da Ana não afeta as demais sessões (Carlos continua consultando).
 */
static void testHistSessaoInexistente() {
    CenarioHistorico c;
    double percentual = -1;
    assert(c.presenca.query(c.outubro(), 999).size() == 0 && "sessao inexistente: Colecao vazia");
    bool okInexistente = c.presenca.attendancePercentage(c.outubro(), percentual, 999);
    assert(!okInexistente && "sessao inexistente: percentual false");
    bool removida = c.presenca.remove(c.idAna);
    assert(removida && "(preparo) remove de sessao existente retorna true");
    assert(c.presenca.query(c.outubro(), c.idAna).size() == 0 && "sessao removida: Colecao vazia");
    bool okRemovida = c.presenca.attendancePercentage(c.outubro(), percentual, c.idAna);
    assert(!okRemovida && "sessao removida: percentual false");
    assert(c.presenca.query(c.outubro(0, 2), c.idCarlos).size() == 6 && "remover a sessao da Ana nao afeta as demais (Carlos)");
}

// ---------- Alertar justificativa pendente ----------

struct CenarioAlertas {
    Usuario sistema, carlos, outroChefe, jovem;
    Sessao sessaoSistema, sessaoCarlos, sessaoOutroChefe, sessaoJovem;
    ServicoPresenca presenca;
    int idSistema, idCarlos, idOutroChefe, idJovem;

    CenarioAlertas()
        : sistema(0, DadosUsuario("Sistema", Data(1980, 1, 1), "UEL", "Sistema", 0)),
          carlos(2, DadosUsuario("Carlos", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2)),
          outroChefe(4, DadosUsuario("Outro Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3)),
          jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)),
          sessaoSistema(sistema), sessaoCarlos(carlos), sessaoOutroChefe(outroChefe), sessaoJovem(jovem) {
        Repositorio& repositorio = Repositorio::instance();
        repositorio.clear();
        repositorio.usuarios.push_back(carlos);
        repositorio.usuarios.push_back(outroChefe);
        repositorio.usuarios.push_back(jovem);
        idSistema = presenca.add(sessaoSistema);
        idCarlos = presenca.add(sessaoCarlos);
        idOutroChefe = presenca.add(sessaoOutroChefe);
        idJovem = presenca.add(sessaoJovem);
    }
};

// CT-01: justificativa pendente da secao 2; Carlos e o Chefe responsavel.
// O registro e a analise da justificativa sao externos a esta API.
static void testAlertasCT01GerarEConsultar() {
    CenarioAlertas c;
    Repositorio::instance().justificativas.push_back(Justificativa(20, 7, 12, 2));
    bool rodou = c.presenca.generatePendingAlerts(c.idSistema);
    assert(rodou && "Alertas CT-01: a rotina executada pela sessao do Sistema retorna true");
    Colecao<Alerta> alertas = c.presenca.query(FiltroAlerta(), c.idCarlos);
    assert(alertas.size() == 1 && "Alertas CT-01: a Colecao do Carlos tem 1 alerta");
    Iterador<Alerta> iterador = alertas.iterator();
    assert(iterador.hasNext() && "Alertas CT-01: o iterador tem o alerta");
    Alerta alerta = iterador.next();   // avanca o iterador FORA do assert
    assert(alerta.recipientId() == 2 && "Alertas CT-01: o destinatario e o Carlos");
    assert(alerta.justificationId() == 20 && "Alertas CT-01: o alerta indica a justificativa 20");
    assert(!iterador.hasNext() && "Alertas CT-01: nao ha segundo alerta");
    assert(c.presenca.query(FiltroAlerta(), c.idOutroChefe).size() == 0 && "Alertas CT-01: Chefe de outra secao nao recebe o aviso");
    assert(c.presenca.query(FiltroAlerta(), c.idJovem).size() == 0 && "Alertas CT-01: o Jovem nao recebe alertas de Chefe");
}

// CT-02: a rotina roda novamente e a justificativa continua pendente.
static void testAlertasCT02SemDuplicar() {
    CenarioAlertas c;
    Repositorio& repositorio = Repositorio::instance();
    repositorio.justificativas.push_back(Justificativa(20, 7, 12, 2));
    bool primeira = c.presenca.generatePendingAlerts(c.idSistema);
    Colecao<Alerta> antes = c.presenca.query(FiltroAlerta(), c.idCarlos);
    assert(primeira && antes.size() == 1 && "Alertas CT-02 (preparo): a primeira rotina gera 1 alerta");
    bool segunda = c.presenca.generatePendingAlerts(c.idSistema);
    Colecao<Alerta> depois = c.presenca.query(FiltroAlerta(), c.idCarlos);
    assert(segunda && "Alertas CT-02: a rotina repetida retorna true");
    assert(depois.size() == 1 && "Alertas CT-02: continua 1 alerta na Colecao do Carlos");
    assert(repositorio.alertas.size() == 1 && "Alertas CT-02: o Repositorio nao ganhou alerta duplicado");
    assert(depois.at(0).recipientId() == 2 && "Alertas CT-02: mesmo destinatario");
    assert(depois.at(0).justificationId() == 20 && "Alertas CT-02: mesma justificativa");
}

// CT-02: justificativa ja tratada deixa de aparecer na consulta de pendencias.
static void testAlertasCT02JustificativaTratada() {
    string estados[] = {"Aceita", "Recusada"};
    for (const string& estado : estados) {
        CenarioAlertas c;
        Repositorio& repositorio = Repositorio::instance();
        repositorio.justificativas.push_back(Justificativa(20, 7, 12, 2));
        bool preparado = c.presenca.generatePendingAlerts(c.idSistema);
        Colecao<Alerta> antes = c.presenca.query(FiltroAlerta(), c.idCarlos);
        assert(preparado && antes.size() == 1 && "Alertas CT-02 (preparo): o alerta existe enquanto a justificativa esta pendente");
        repositorio.justificativas[0].setStatus(estado);   // o tratamento acontece fora desta API
        bool ok = c.presenca.generatePendingAlerts(c.idSistema);
        Colecao<Alerta> depois = c.presenca.query(FiltroAlerta(), c.idCarlos);
        assert(ok && "Alertas CT-02: a rotina retorna true");
        assert(depois.size() == 0 && "Alertas CT-02: justificativa Aceita ou Recusada deixa a Colecao vazia");
    }
}

// Alternativa: sem pendencias (ou ja tratadas), nenhum alerta e gerado.
static void testAlertasSemPendencias() {
    CenarioAlertas c;
    Repositorio& repositorio = Repositorio::instance();
    bool semNada = c.presenca.generatePendingAlerts(c.idSistema);
    assert(semNada && "sem justificativas: a rotina retorna true");
    assert(repositorio.alertas.empty() && "sem justificativas: nenhum alerta e gerado");
    repositorio.justificativas.push_back(Justificativa(21, 7, 12, 2, "Aceita"));
    bool jaTratada = c.presenca.generatePendingAlerts(c.idSistema);
    assert(jaTratada && "justificativa ja tratada: a rotina retorna true");
    assert(repositorio.alertas.empty() && "justificativa ja tratada: nenhum alerta e gerado");
    assert(c.presenca.query(FiltroAlerta(), c.idCarlos).size() == 0 && "sem pendencias: Colecao do Carlos vazia");
}

// A rotina so roda na sessao do Sistema.
static void testAlertasSoSessaoDoSistema() {
    CenarioAlertas c;
    Repositorio& repositorio = Repositorio::instance();
    repositorio.justificativas.push_back(Justificativa(20, 7, 12, 2));
    bool comoChefe = c.presenca.generatePendingAlerts(c.idCarlos);
    bool comoJovem = c.presenca.generatePendingAlerts(c.idJovem);
    bool semSessao = c.presenca.generatePendingAlerts(999);
    assert(!comoChefe && "sessao de Chefe: generatePendingAlerts retorna false");
    assert(!comoJovem && "sessao de Jovem: generatePendingAlerts retorna false");
    assert(!semSessao && "sessao inexistente: generatePendingAlerts retorna false");
    assert(repositorio.alertas.empty() && "fora da sessao do Sistema nenhum alerta e gerado");
    bool comoSistema = c.presenca.generatePendingAlerts(c.idSistema);
    assert(comoSistema && "controle: a sessao do Sistema roda a rotina");
    assert(repositorio.alertas.size() == 1 && "controle: o alerta so existe depois da rotina do Sistema");
}

// Teste final: enche a memoria de verdade chamando enroll (push_back) ate faltar memoria.
static void testFinalFaltaDeMemoria() {
    Atividade atividade;
    try {
        while (true)
            atividade.enroll(1);
    } catch (const bad_alloc&) {
        cout << "Erro: faltou memoria no enroll (" << atividade.members().size() << " chamadas).\n";
    }
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
    cout << "\n=== FUNCIONAIS: Registrar presenca, ausencia ou atraso ===\n";
    executar("CT-01 registro valido", testPresCT01RegistroValido);
    executar("CT-02 integrante ja registrado", testPresCT02Duplicado);
    executar("atividade em andamento", testPresAtividadeEmAndamento);
    executar("lote invalido (tudo ou nada)", testPresLoteInvalido);
    executar("Chefe de outra secao", testPresOutraSecao);
    executar("atividade cancelada", testPresCancelada);
    executar("sessao de Jovem", testPresSessaoJovem);
    cout << "\n=== FUNCIONAIS: Consultar historico de participacao ===\n";
    executar("CT-01 Ana", testHistCT01Ana);
    executar("CT-02 Ana nao ve outro integrante", testHistCT02AnaNaoVeOutro);
    executar("Dirigente consulta a secao", testHistDirigenteSecao);
    executar("Chefe de outra secao", testHistChefeOutraSecao);
    executar("alvo que nao e Jovem", testHistAlvoNaoJovem);
    executar("sem registros no periodo", testHistSemRegistros);
    executar("atividade cancelada", testHistCancelada);
    executar("sessao inexistente ou removida", testHistSessaoInexistente);
    cout << "\n=== FUNCIONAIS: Alertar justificativa pendente ===\n";
    executar("CT-01 gerar e consultar", testAlertasCT01GerarEConsultar);
    executar("CT-02 repetir sem duplicar", testAlertasCT02SemDuplicar);
    executar("CT-02 justificativa tratada", testAlertasCT02JustificativaTratada);
    executar("sem pendencias", testAlertasSemPendencias);
    executar("rotina so na sessao do Sistema", testAlertasSoSessaoDoSistema);
    cout << "\n=== FUNCIONAL FINAL: falha de memoria ===\n";
    executar("enroll em while ate acabar a memoria", testFinalFaltaDeMemoria);
    Repositorio::instance().clear();
}