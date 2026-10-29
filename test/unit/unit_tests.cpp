#include "unit_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
#include "../../src/moduloConcre/ServicoAtividades.h"
#include "../../src/moduloConcre/Repositorio.h"

using namespace std;
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

int runUnitTests() {
    falhas = 0;
    testRegistrarDadosValidos();
    testRegistrarCamposInvalidos();
    testRegistrarPermissoes();
    testRegistrarDuplicidade();
    testAddRegra();
    testValidStartTime();
    testAtividadeEnroll();
    testRegistrarAtividadeValida();
    testRegistrarAtividadeCamposInvalidos();
    testRegistrarAtividadePermissoes();
    testAddRegraAtividade();
    Repositorio::instance().clear();
    return falhas;
}
