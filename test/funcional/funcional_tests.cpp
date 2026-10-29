#include "funcional_tests.h"
#include <iostream>
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

int runFunctionalTests() {
    falhas = 0;
    testCT01CadastroValido();
    testCT02CadastroDuplicado();
    Repositorio::instance().clear();
    return falhas;
}
