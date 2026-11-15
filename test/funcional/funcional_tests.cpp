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
}