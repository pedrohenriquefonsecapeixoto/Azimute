#include "unit_tests.h"
#include <iostream>
#include "../../include/Regra.h"
#include "../../src/moduloConcre/ServicoUsuarios.h"
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

int runUnitTests() {
    falhas = 0;
    testRegistrarDadosValidos();
    testRegistrarCamposInvalidos();
    testRegistrarPermissoes();
    testRegistrarDuplicidade();
    testAddRegra();
    Repositorio::instance().clear();
    return falhas;
}
