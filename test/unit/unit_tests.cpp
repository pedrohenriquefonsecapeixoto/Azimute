#include "unit_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
#include "../../src/moduloConcre/ServicoAtividades.h"
#include "../../src/moduloConcre/Repositorio.h"
#include "../../src/moduloConcre/ServicoPresenca.h"
#include <string>
#include <vector>
#include <ctime>
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

struct Cadastro {
    Usuario dirigente{1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0)};
    Sessao sessaoDirigente{dirigente};
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    Cadastro() { Repositorio::instance().clear(); }
};

static void testRegistrarDadosValidos() {
    Cadastro cadastro;
    const string papeis[] = {"Jovem", "Chefe Escotista", "Dirigente da UEL"};
    for (int i = 0; i < 3; ++i) {
        Usuario novo;
        DadosUsuario dados("Pessoa " + to_string(i), Data(2000, 1, 1), "UEL", papeis[i], 2);
        bool ok = cadastro.usuarios.registrar(dados, novo, cadastro.idDirigenteUsuarios);
        const Usuario* salvo = Repositorio::instance().findUser(novo.id());
        verify(ok && novo.id() >= 100 && salvo && igual(novo, *salvo)
            && novo.details().name() == dados.name() && novo.details().birth() == dados.birth()
            && novo.details().bond() == dados.bond() && novo.role() == dados.role()
            && novo.sectionId() == dados.sectionId(), "registrar preenche e grava usuario de papel permitido: " + papeis[i]);
    }
}

static void testRegistrarCamposInvalidos() {
    Cadastro cadastro;
    const DadosUsuario invalidos[] = {
        DadosUsuario("", Data(2012, 5, 20), "UEL", "Jovem", 2),
        DadosUsuario("Ana", Data(), "UEL", "Jovem", 2),
        DadosUsuario("Ana", Data(2012, 2, 31), "UEL", "Jovem", 2),
        DadosUsuario("Ana", Data(2012, 5, 20), "", "Jovem", 2),
        DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Administrador", 2)
    };
    Usuario original(99, DadosUsuario("Saida preservada", Data(1990, 1, 1), "Outro", "Chefe Escotista", 4));
    for (int i = 0; i < 5; ++i) {
        Usuario novo = original;
        verify(!cadastro.usuarios.registrar(invalidos[i], novo, cadastro.idDirigenteUsuarios)
            && igual(novo, original) && Repositorio::instance().usuarios.empty(),
            "registrar recusa campo invalido sem alterar armazenamento ou saida: " + to_string(i + 1));
    }
}

static void testRegistrarPermissoes() {
    Cadastro cadastro;
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Usuario jovem(3, DadosUsuario("Jovem", Data(2012, 1, 1), "UEL", "Jovem", 2));
    Sessao sessaoChefe(chefe), sessaoJovem(jovem);
    int ids[] = {cadastro.usuarios.add(sessaoChefe), cadastro.usuarios.add(sessaoJovem), -1, 999};
    Usuario original(99, DadosUsuario("Saida", Data(1990, 1, 1), "UEL", "Jovem", 2));
    DadosUsuario dados("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2);
    for (int id : ids) {
        Usuario novo = original;
        verify(!cadastro.usuarios.registrar(dados, novo, id) && igual(novo, original)
            && Repositorio::instance().usuarios.empty(), "registrar recusa papel ou sessao sem autorizacao: " + to_string(id));
    }
    cadastro.usuarios.remove(cadastro.idDirigenteUsuarios);
    Usuario novo = original;
    verify(!cadastro.usuarios.registrar(dados, novo, cadastro.idDirigenteUsuarios)
        && igual(novo, original) && Repositorio::instance().usuarios.empty(), "registrar recusa sessao removida");
}

static void testRegistrarDuplicidade() {
    Cadastro cadastro;
    Usuario novo;
    DadosUsuario dados("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2);
    bool preparado = cadastro.usuarios.registrar(dados, novo, cadastro.idDirigenteUsuarios);
    Usuario original = novo;
    bool duplicado = cadastro.usuarios.registrar(DadosUsuario("Ana", Data(2012, 5, 20), "Outra UEL", "Chefe Escotista", 3), novo, cadastro.idDirigenteUsuarios);
    verify(preparado && !duplicado && igual(novo, original) && Repositorio::instance().usuarios.size() == 1,
        "registrar identifica duplicidade por nome e nascimento mesmo com outros campos diferentes");
    verify(cadastro.usuarios.registrar(DadosUsuario("Ana", Data(2012, 5, 21), "UEL", "Jovem", 2), novo, cadastro.idDirigenteUsuarios),
        "mesmo nome com nascimento diferente nao e duplicado");
    verify(cadastro.usuarios.registrar(DadosUsuario("Beto", Data(2012, 5, 20), "UEL", "Jovem", 2), novo, cadastro.idDirigenteUsuarios),
        "mesmo nascimento com nome diferente nao e duplicado");
}

class RegraVinculo : public Regra<DadosUsuario> {
public:
    bool allows(const DadosUsuario& dados) const override { 
        return dados.bond() == "UEL"; 
    }
};
class RegraSecao : public Regra<DadosUsuario> {
public:
    bool allows(const DadosUsuario& dados) const override { 
        return dados.sectionId() == 2; 
    }
};

static void testAddRegra() {
    Cadastro cadastro;
    RegraVinculo vinculo;
    RegraSecao secao;
    cadastro.usuarios.add(vinculo);
    cadastro.usuarios.add(secao);
    Usuario original(99, DadosUsuario("Saida", Data(1990, 1, 1), "UEL", "Jovem", 2));
    Usuario novo = original;
    verify(!cadastro.usuarios.registrar(DadosUsuario("Ana", Data(2012, 5, 20), "Outro", "Jovem", 2), novo, cadastro.idDirigenteUsuarios)
        && igual(novo, original) && Repositorio::instance().usuarios.empty(), "add(regra) faz registrar consultar a primeira regra");
    verify(!cadastro.usuarios.registrar(DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 3), novo, cadastro.idDirigenteUsuarios)
        && igual(novo, original) && Repositorio::instance().usuarios.empty(), "registrar consulta tambem a segunda regra");
    verify(cadastro.usuarios.registrar(DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2), novo, cadastro.idDirigenteUsuarios)
        && Repositorio::instance().usuarios.size() == 1, "registrar aceita dados que atendem a todas as regras adicionadas");
}


// ---------- Cadastrar atividade/evento ----------

static bool igualAtv(const Atividade& a, const Atividade& b) {
    return a.id() == b.id() && a.details().title() == b.details().title()
        && a.details().date() == b.details().date() && a.details().startTime() == b.details().startTime()
        && a.details().place() == b.details().place() && a.details().type() == b.details().type()
        && a.details().branch() == b.details().branch() && a.sectionId() == b.sectionId()
        && a.status() == b.status() && a.members() == b.members();
}

static DadosAtividade dadosAtv(const string& titulo = "Acampamento", const string& horario = "08:30",
                               const string& tipo = "Acampamento") {
    return DadosAtividade(titulo, Data(2026, 11, 15), horario, "Sede", tipo, "Escoteiro");
}

struct CadastroAtv {
    Usuario chefe{2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2)};
    Sessao sessaoChefe{chefe};
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    CadastroAtv() {
        Repositorio& repositorio = Repositorio::instance();
        repositorio.clear();
        repositorio.usuarios.push_back(chefe);
        repositorio.usuarios.push_back(Usuario(10, DadosUsuario("Jovem A", Data(2012, 1, 1), "UEL", "Jovem", 2)));
        repositorio.usuarios.push_back(Usuario(11, DadosUsuario("Jovem B", Data(2012, 2, 2), "UEL", "Jovem", 2)));
        repositorio.usuarios.push_back(Usuario(12, DadosUsuario("Jovem C", Data(2012, 3, 3), "UEL", "Jovem", 3)));
    }
};

static void testValidStartTime() {
    const string validos[] = {"00:00", "08:30", "23:59", "19:05"};
    const string invalidos[] = {"", "25:00", "8:00", "24:00", "12:60", "12-30", "ab:cd", "08:300"};
    for (const string& h : validos)
        verify(dadosAtv("Titulo", h).validStartTime(), "validStartTime aceita " + h);
    for (const string& h : invalidos)
        verify(!dadosAtv("Titulo", h).validStartTime(), "validStartTime recusa \"" + h + "\"");
}

static void testAtividadeEnroll() {
    Atividade atv(5, dadosAtv(), 2, "Confirmada");
    verify(atv.id() == 5 && atv.sectionId() == 2 && atv.status() == "Confirmada"
        && atv.details().title() == "Acampamento" && atv.members().empty(), "Atividade guarda id, secao, situacao e dados");
    atv.enroll(10);
    atv.enroll(11);
    verify(atv.members().size() == 2 && atv.members()[0] == 10 && atv.members()[1] == 11, "enroll adiciona integrantes em ordem");
    Atividade copia(atv);
    atv.enroll(12);
    verify(copia.members().size() == 2 && atv.members().size() == 3, "copia de Atividade e profunda");
    Atividade atribuida;
    atribuida = atv;
    verify(igualAtv(atribuida, atv), "atribuicao de Atividade copia todos os campos");
}

static void testRegistrarAtividadeValida() {
    CadastroAtv cadastro;
    DadosAtividade dados = dadosAtv();
    Atividade nova;
    bool ok = cadastro.atividades.registrar(dados, nova, cadastro.idChefe);
    const Atividade* salva = Repositorio::instance().atividades.empty() ? nullptr : &Repositorio::instance().atividades[0];
    verify(ok && nova.id() >= 100 && salva && igualAtv(nova, *salva) && Repositorio::instance().atividades.size() == 1,
        "registrar preenche e grava a atividade");
    verify(nova.status() == "Confirmada" && nova.sectionId() == 2 && nova.details().title() == "Acampamento"
        && nova.details().startTime() == "08:30", "registrar define situacao Confirmada, secao do Chefe e dados");
    verify(nova.members().size() == 2 && nova.members()[0] == 10 && nova.members()[1] == 11,
        "registrar inscreve so os Jovens da secao do Chefe");
    Atividade outra;
    cadastro.atividades.registrar(dadosAtv("Outra"), outra, cadastro.idChefe);
    verify(outra.id() != nova.id(), "cada atividade recebe id proprio");
}

static void testRegistrarAtividadeCamposInvalidos() {
    CadastroAtv cadastro;
    const DadosAtividade invalidos[] = {
        DadosAtividade("", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(), "08:30", "Sede", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(2026, 2, 31), "08:30", "Sede", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(2026, 11, 15), "25:00", "Sede", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(2026, 11, 15), "8:00", "Sede", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(2026, 11, 15), "08:30", "", "Acampamento", "Escoteiro"),
        DadosAtividade("T", Data(2026, 11, 15), "08:30", "Sede", "", "Escoteiro"),
        DadosAtividade("T", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "")
    };
    Atividade original(99, dadosAtv("Saida preservada"), 4, "Realizada");
    for (int i = 0; i < 8; ++i) {
        Atividade nova = original;
        verify(!cadastro.atividades.registrar(invalidos[i], nova, cadastro.idChefe)
            && igualAtv(nova, original) && Repositorio::instance().atividades.empty(),
            "registrar recusa campo invalido sem alterar armazenamento ou saida: " + to_string(i + 1));
    }
}

static void testRegistrarAtividadePermissoes() {
    CadastroAtv cadastro;
    Usuario jovem(3, DadosUsuario("Jovem", Data(2012, 1, 1), "UEL", "Jovem", 2));
    Usuario dirigente(1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoJovem(jovem), sessaoDirigente(dirigente);
    int ids[] = {cadastro.atividades.add(sessaoJovem), cadastro.atividades.add(sessaoDirigente), -1, 999};
    Atividade original(99, dadosAtv("Saida"), 4, "Realizada");
    for (int id : ids) {
        Atividade nova = original;
        verify(!cadastro.atividades.registrar(dadosAtv(), nova, id) && igualAtv(nova, original)
            && Repositorio::instance().atividades.empty(), "registrar recusa sessao que nao e de Chefe: " + to_string(id));
    }
    cadastro.atividades.remove(cadastro.idChefe);
    Atividade nova = original;
    verify(!cadastro.atividades.registrar(dadosAtv(), nova, cadastro.idChefe) && igualAtv(nova, original)
        && Repositorio::instance().atividades.empty(), "registrar recusa sessao removida");
}

class RegraTipoAtv : public Regra<DadosAtividade> {
public:
    bool allows(const DadosAtividade& dados) const override { 
        return dados.type() != "Proibido"; 
    }
};
class RegraRamoAtv : public Regra<DadosAtividade> {
public:
    bool allows(const DadosAtividade& dados) const override { 
        return dados.branch() == "Escoteiro"; 
    }
};

static void testAddRegraAtividade() {
    CadastroAtv cadastro;
    RegraTipoAtv tipo;
    RegraRamoAtv ramo;
    cadastro.atividades.add(tipo);
    cadastro.atividades.add(ramo);
    Atividade original(99, dadosAtv("Saida"), 4, "Realizada");
    Atividade nova = original;
    verify(!cadastro.atividades.registrar(dadosAtv("T", "08:30", "Proibido"), nova, cadastro.idChefe)
        && igualAtv(nova, original) && Repositorio::instance().atividades.empty(), "add(regra) faz registrar consultar a primeira regra");
    verify(!cadastro.atividades.registrar(DadosAtividade("T", Data(2026, 11, 15), "08:30", "Sede", "Acampamento", "Lobinho"), nova, cadastro.idChefe)
        && igualAtv(nova, original) && Repositorio::instance().atividades.empty(), "registrar consulta tambem a segunda regra");
    verify(cadastro.atividades.registrar(dadosAtv(), nova, cadastro.idChefe)
        && Repositorio::instance().atividades.size() == 1, "registrar aceita dados que atendem a todas as regras adicionadas");
}

// ---------- Consultar atividade/evento ----------
struct ConsultaAtv {
    Usuario chefe{2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2)};
    Sessao sessaoChefe{chefe};
    ServicoAtividades atividades;
    int idChefe = atividades.add(sessaoChefe);
    ConsultaAtv() { Repositorio::instance().clear(); }
    ~ConsultaAtv() { Repositorio::instance().clear(); }
};

void prepararAtividade(int id, const Data& data, const string& situacao = "Confirmada", int idSecao = 2) {
    DadosAtividade dados("Atividade " + to_string(id), data, "08:30", "Sede", "Reuniao", "Escoteiro", "Descricao");
    Repositorio::instance().atividades.push_back(Atividade(id, dados, idSecao, situacao));
}

bool idsIguais(const Colecao<Atividade>& lista, const vector<int>& esperados) {
    vector<int> ids;
    for (auto it = lista.iterator(); it.hasNext();) ids.push_back(it.next().id());
    return lista.size() == static_cast<int>(esperados.size()) && ids == esperados;
}

void testFiltroAtividade() {
    FiltroAtividade filtro(Data(2026, 10, 1), Data(2026, 10, 31), "Confirmada", 2);
    verify(filtro.start() == Data(2026, 10, 1) && filtro.end() == Data(2026, 10, 31)
        && filtro.status() == "Confirmada" && filtro.sectionId() == 2,
        "FiltroAtividade guarda periodo, situacao e secao");
    FiltroAtividade padrao(Data(2026, 10, 1), Data(2026, 10, 31));
    verify(padrao.status().empty() && padrao.sectionId() == 0,
        "filtros opcionais nao restringem situacao nem selecionam outra secao");
}

void testQueryPeriodo() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 9, 30));
    prepararAtividade(11, Data(2026, 10, 1));
    prepararAtividade(12, Data(2026, 10, 15));
    prepararAtividade(13, Data(2026, 10, 31));
    prepararAtividade(14, Data(2026, 11, 1));
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), consulta.idChefe), {11, 12, 13}),
        "query inclui os dois limites do periodo e exclui datas fora dele");
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 15), Data(2026, 10, 15)), consulta.idChefe), {12}),
        "query aceita um periodo de um unico dia");
}

void testQuerySituacao() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 10, 3), "Confirmada");
    prepararAtividade(11, Data(2026, 10, 4), "Cancelada");
    prepararAtividade(12, Data(2026, 10, 5), "Realizada");
    const string situacoes[] = {"Confirmada", "Cancelada", "Realizada"};
    for (int indice = 0; indice < 3; ++indice)
        verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31), situacoes[indice]), consulta.idChefe), {10 + indice}),
            "query filtra situacao " + situacoes[indice]);
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), consulta.idChefe), {10, 11, 12}),
        "query sem situacao retorna todas as situacoes do periodo");
}

void testQuerySecao() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 10, 3), "Confirmada", 2);
    prepararAtividade(11, Data(2026, 10, 3), "Confirmada", 3);
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31)), consulta.idChefe), {10}),
        "query sem secao explicita restringe o resultado a secao do Chefe");
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31), "", 2), consulta.idChefe), {10}),
        "query aceita filtro explicito da propria secao");
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31), "", 3), consulta.idChefe), {}),
        "query recusa filtro explicito de outra secao");
}

void testQueryPermissoes() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 10, 3));
    Usuario jovem(3, DadosUsuario("Jovem", Data(2012, 1, 1), "UEL", "Jovem", 2));
    Usuario dirigente(1, DadosUsuario("Dirigente", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 2));
    Sessao sessaoJovem(jovem), sessaoDirigente(dirigente);
    int ids[] = {consulta.atividades.add(sessaoJovem), consulta.atividades.add(sessaoDirigente), -1, 999};
    FiltroAtividade filtro(Data(2026, 10, 1), Data(2026, 10, 31));
    for (int id : ids)
        verify(idsIguais(consulta.atividades.query(filtro, id), {}), "query recusa papel ou sessao nao autorizado: " + to_string(id));
    bool removida = consulta.atividades.remove(consulta.idChefe);
    verify(removida && idsIguais(consulta.atividades.query(filtro, consulta.idChefe), {}),
        "query recusa sessao removida");
}

void testQuerySemCorrespondencia() {
    ConsultaAtv consulta;
    FiltroAtividade filtro(Data(2026, 10, 1), Data(2026, 10, 31));
    verify(idsIguais(consulta.atividades.query(filtro, consulta.idChefe), {}), "query com repositorio vazio devolve colecao vazia");
    prepararAtividade(10, Data(2026, 11, 1));
    verify(idsIguais(consulta.atividades.query(filtro, consulta.idChefe), {}), "query sem data correspondente devolve colecao vazia");
    prepararAtividade(11, Data(2026, 10, 1), "Realizada");
    verify(idsIguais(consulta.atividades.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31), "Cancelada"), consulta.idChefe), {}),
        "query sem situacao correspondente devolve colecao vazia");
}

void testQueryCombinaFiltros() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 10, 3), "Confirmada", 2);
    prepararAtividade(11, Data(2026, 11, 3), "Confirmada", 2);
    prepararAtividade(12, Data(2026, 10, 3), "Realizada", 2);
    prepararAtividade(13, Data(2026, 10, 3), "Confirmada", 3);
    Consulta<Atividade, FiltroAtividade>& contrato = consulta.atividades;
    verify(idsIguais(contrato.query(FiltroAtividade(Data(2026, 10, 1), Data(2026, 10, 31), "Confirmada", 2), consulta.idChefe), {10}),
        "query combina periodo, situacao e secao pelo contrato Consulta");
}

void testQueryResultadoIndependente() {
    ConsultaAtv consulta;
    prepararAtividade(10, Data(2026, 10, 3));
    FiltroAtividade filtro(Data(2026, 10, 1), Data(2026, 10, 31));
    Colecao<Atividade> lista = consulta.atividades.query(filtro, consulta.idChefe);
    verify(Repositorio::instance().atividades.size() == 1 && Repositorio::instance().atividades[0].id() == 10,
        "query preserva a atividade armazenada");
    Repositorio::instance().clear();
    verify(idsIguais(lista, {10}) && lista.at(0).details().title() == "Atividade 10",
        "resultado e copia independente do repositorio");
}


// ---------- Registrar presença, ausência ou atraso ----------
static void testParticipacao() {
    Participacao p(7, "Presente");
    verify(p.memberId() == 7 && p.status() == "Presente" && p.activityId() == 0 && p.recordedAt() == 0,
        "Participacao nasce sem atividade e sem horario de registro");
    p.attach(12, 1000);
    verify(p.activityId() == 12 && p.recordedAt() == 1000 && p.memberId() == 7 && p.status() == "Presente",
        "attach grava atividade e horario sem mexer no resto");
    Participacao copia(p);
    p.attach(13, 2000);
    verify(copia.activityId() == 12 && copia.recordedAt() == 1000, "copia de Participacao e independente");
    Participacao atribuida;
    atribuida = p;
    verify(atribuida.memberId() == 7 && atribuida.activityId() == 13 && atribuida.recordedAt() == 2000
        && atribuida.status() == "Presente", "atribuicao de Participacao copia todos os campos");
}

static void testListMembers() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Usuario chefeOutro(4, DadosUsuario("Chefe 3", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3));
    Usuario jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(chefeOutro);
    r.usuarios.push_back(jovem);
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(9, DadosUsuario("Caio", Data(2012, 7, 1), "UEL", "Jovem", 3)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe), sessaoOutro(chefeOutro), sessaoJovem(jovem);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    int idOutro = presenca.add(sessaoOutro);
    int idJovem = presenca.add(sessaoJovem);
    Colecao<Usuario> lista = presenca.listMembers(12, idChefe);
    verify(lista.size() == 2 && lista.at(0).id() == 7 && lista.at(1).id() == 8,
        "listMembers devolve os Jovens inscritos na atividade da secao do Chefe");
    r.atividades[0].enroll(9); // Jovem de outra secao
    r.atividades[0].enroll(2); // Chefe inscrito
    verify(presenca.listMembers(12, idChefe).size() == 2,
        "listMembers ignora inscritos que nao sao Jovens da secao da atividade");
    verify(presenca.listMembers(12, idOutro).size() == 0, "listMembers vazio para Chefe de outra secao");
    verify(presenca.listMembers(12, idJovem).size() == 0, "listMembers vazio para sessao de Jovem");
    verify(presenca.listMembers(999, idChefe).size() == 0, "listMembers vazio para atividade inexistente");
    verify(presenca.listMembers(12, 999).size() == 0, "listMembers vazio para sessao inexistente");
}

static void testRecordIndividual() {
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
    bool ok = presenca.record(12, Participacao(7, "Presente"), idChefe);
    const vector<Participacao>& v = r.participacoes;
    verify(ok && v.size() == 1 && v[0].memberId() == 7 && v[0].activityId() == 12
        && v[0].status() == "Presente" && v[0].recordedAt() > 0, "record individual grava com atividade e horario");
    verify(presenca.record(12, Participacao(8, "Atraso"), idChefe) && v.size() == 2,
        "record individual aceita Atraso para outro integrante");
}

static void testRecordLista() {
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
    verify(presenca.record(12, lista, idChefe) && r.participacoes.size() == 2
        && r.participacoes[0].status() == "Presente" && r.participacoes[1].status() == "Atraso"
        && r.participacoes[0].activityId() == 12 && r.participacoes[1].activityId() == 12,
        "record em lista grava todas as participacoes na atividade");
}

static void testRecordAtividadeNaoEncerrada() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    Atividade futura(13, DadosAtividade("Reuniao", Data(2099, 1, 1), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    futura.enroll(7);
    r.atividades.push_back(futura);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    verify(!presenca.record(13, Participacao(7, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa atividade que ainda nao terminou (inicio + 120 min)");
}

static void testRecordCancelada() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    Atividade cancelada(14, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Cancelada");
    cancelada.enroll(7);
    r.atividades.push_back(cancelada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    verify(!presenca.record(14, Participacao(7, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa atividade cancelada");
}

static void testRecordSituacaoInvalida() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    const string invalidas[] = {"", "presente", "Faltou", "Justificado"};
    for (const string& s : invalidas)
        verify(!presenca.record(12, Participacao(7, s), idChefe) && r.participacoes.empty(),
            "record recusa situacao invalida \"" + s + "\"");
}

static void testRecordNaoInscrito() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(9, DadosUsuario("Caio", Data(2012, 7, 1), "UEL", "Jovem", 3)));
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    verify(!presenca.record(12, Participacao(9, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa Jovem nao inscrito na atividade");
    verify(!presenca.record(12, Participacao(2, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa integrante que nao e Jovem");
    verify(!presenca.record(12, Participacao(999, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa integrante inexistente");
}

static void testRecordDuplicidade() {
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
    verify(presenca.record(12, Participacao(8, "Atraso"), idChefe), "prepara: 8 registrado como Atraso");
    verify(!presenca.record(12, Participacao(8, "Ausente"), idChefe) && r.participacoes.size() == 1
        && r.participacoes[0].status() == "Atraso",
        "record recusa integrante ja registrado e mantem a situacao anterior");
    Colecao<Participacao> repetida;
    repetida.add(Participacao(7, "Presente"));
    repetida.add(Participacao(7, "Ausente"));
    verify(!presenca.record(12, repetida, idChefe) && r.participacoes.size() == 1,
        "record recusa o mesmo integrante duas vezes no mesmo lote");
}

static void testRecordTudoOuNada() {
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
        "lote com situacao invalida no fim nao grava nenhum item");
    Colecao<Participacao> lote2;
    lote2.add(Participacao(7, "Presente"));
    lote2.add(Participacao(9, "Presente"));
    verify(!presenca.record(12, lote2, idChefe) && r.participacoes.empty(),
        "lote com nao inscrito no fim nao grava nenhum item");
    verify(!presenca.record(12, Colecao<Participacao>(), idChefe) && r.participacoes.empty(),
        "lote vazio e recusado");
}

static void testRecordPermissoes() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Usuario chefeOutro(4, DadosUsuario("Chefe 3", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3));
    Usuario jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(chefeOutro);
    r.usuarios.push_back(jovem);
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe), sessaoOutro(chefeOutro), sessaoJovem(jovem);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    int idOutro = presenca.add(sessaoOutro);
    int idJovem = presenca.add(sessaoJovem);
    verify(!presenca.record(12, Participacao(7, "Presente"), idOutro) && r.participacoes.empty(),
        "record recusa Chefe de outra secao");
    verify(!presenca.record(12, Participacao(7, "Presente"), idJovem) && r.participacoes.empty(),
        "record recusa sessao de Jovem");
    verify(!presenca.record(12, Participacao(7, "Presente"), 999) && r.participacoes.empty(),
        "record recusa sessao inexistente");
    verify(!presenca.record(999, Participacao(7, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa atividade inexistente");
    presenca.remove(idChefe);
    verify(!presenca.record(12, Participacao(7, "Presente"), idChefe) && r.participacoes.empty(),
        "record recusa sessao removida");
}

static void testRecordingTime() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Usuario chefeOutro(4, DadosUsuario("Chefe 3", Data(1980, 1, 1), "UEL", "Chefe Escotista", 3));
    Usuario jovem(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(chefeOutro);
    r.usuarios.push_back(jovem);
    Atividade encerrada(12, DadosAtividade("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro"), 2, "Confirmada");
    encerrada.enroll(7);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe), sessaoOutro(chefeOutro), sessaoJovem(jovem);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    int idOutro = presenca.add(sessaoOutro);
    int idJovem = presenca.add(sessaoJovem);
    int minutos = -5;
    verify(!presenca.recordingTime(12, minutos, idChefe) && minutos == -5,
        "recordingTime retorna false sem registro e nao altera a saida");
    presenca.record(12, Participacao(7, "Presente"), idChefe);
    minutos = -5;
    verify(presenca.recordingTime(12, minutos, idChefe) && minutos > 0,
        "recordingTime retorna true com minutos desde o fim da atividade");
    minutos = -5;
    verify(!presenca.recordingTime(12, minutos, idOutro) && minutos == -5,
        "recordingTime recusa Chefe de outra secao");
    verify(!presenca.recordingTime(12, minutos, idJovem), "recordingTime recusa Jovem");
    verify(!presenca.recordingTime(999, minutos, idChefe), "recordingTime recusa atividade inexistente");
}

static void testRecordingTimeUsaPrimeiroRegistro() {
    Repositorio& r = Repositorio::instance();
    r.clear();
    Usuario chefe(2, DadosUsuario("Chefe", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    r.usuarios.push_back(chefe);
    r.usuarios.push_back(Usuario(7, DadosUsuario("Ana", Data(2012, 5, 20), "UEL", "Jovem", 2)));
    r.usuarios.push_back(Usuario(8, DadosUsuario("Beto", Data(2012, 6, 1), "UEL", "Jovem", 2)));
    DadosAtividade dados("Reuniao", Data(2026, 10, 3), "08:00", "Sede", "Reuniao", "Escoteiro");
    Atividade encerrada(12, dados, 2, "Confirmada");
    encerrada.enroll(7);
    encerrada.enroll(8);
    r.atividades.push_back(encerrada);
    Sessao sessaoChefe(chefe);
    ServicoPresenca presenca;
    int idChefe = presenca.add(sessaoChefe);
    time_t fim = 0;
    bool temFim = dados.endTime(120, fim);
    Participacao primeira(7, "Presente");
    Participacao segunda(8, "Presente");
    primeira.attach(12, fim + 30 * 60);
    segunda.attach(12, fim + 90 * 60);
    r.participacoes.push_back(segunda); // fora de ordem de proposito
    r.participacoes.push_back(primeira);
    int minutos = -1;
    verify(temFim && presenca.recordingTime(12, minutos, idChefe) && minutos == 30,
        "recordingTime mede ate o primeiro registro (30 min)");
}


int runUnitTests() {
    falhas = 0;
    cout << "\n=== UNITARIOS: Cadastrar usuario ===\n";
    testRegistrarDadosValidos();
    testRegistrarCamposInvalidos();
    testRegistrarPermissoes();
    testRegistrarDuplicidade();
    testAddRegra();
    cout << "\n=== UNITARIOS: Cadastrar atividade/evento ===\n";
    testValidStartTime();
    testAtividadeEnroll();
    testRegistrarAtividadeValida();
    testRegistrarAtividadeCamposInvalidos();
    testRegistrarAtividadePermissoes();
    testAddRegraAtividade();
    cout << "\n=== UNITARIOS: Consultar atividade/evento ===\n";
    testFiltroAtividade();
    testQueryPeriodo();
    testQuerySituacao();
    testQuerySecao();
    testQueryPermissoes();
    testQuerySemCorrespondencia();
    testQueryCombinaFiltros();
    testQueryResultadoIndependente();
    cout << "\n=== UNITARIOS: Registrar presenca,ausencia ou atraso ===\n";
    testParticipacao();
    testListMembers();
    testRecordIndividual();
    testRecordLista();
    testRecordAtividadeNaoEncerrada();
    testRecordCancelada();
    testRecordSituacaoInvalida();
    testRecordNaoInscrito();
    testRecordDuplicidade();
    testRecordTudoOuNada();
    testRecordPermissoes();
    testRecordingTime();
    testRecordingTimeUsaPrimeiroRegistro();
    Repositorio::instance().clear();
    return falhas;
}
