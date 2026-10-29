#include "Menu.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include "Usuario.h"
#include "Entrada.h"
#include "ServicoUsuarios.h"

using namespace std;


string texto(const char* pergunta) {
    cout << pergunta;
    string valor;
    getline(cin, valor);
    return valor;
}

int numero(const char* pergunta, int minimo = numeric_limits<int>::min(),
           int maximo = numeric_limits<int>::max()) {
    while (cin) {
        istringstream entrada(texto(pergunta));
        if (!cin) break;
        int valor;
        char sobra;
        if (!(entrada >> valor) || (entrada >> sobra)) {
            cout << "Numero invalido.\n";
            continue;
        }
        if (valor < minimo || valor > maximo) {
            cout << "Informe um valor entre " << minimo << " e " << maximo << ".\n";
            continue;
        }
        return valor;
    }
    return -1;
}

Data data() {
    int ano = numero("Ano: ", 1);
    if (!cin) return Data();
    int mes = numero("Mes: ", 1, 12);
    if (!cin) return Data();
    while (cin) {
        int dia = numero("Dia: ", 1, 31);
        if (!cin) break;
        Data valor(ano, mes, dia);
        if (valor.valid()) return valor;
        cout << "Dia invalido para o mes e ano informados.\n";
    }
    return Data();
}

DadosUsuario dadosUsuario() {
    string nome = texto("Nome: ");
    cout << "Data de nascimento\n";
    Data nascimento = data();
    string vinculo = texto("Vinculo: ");
    cout << "1 Jovem\n2 Chefe Escotista\n3 Dirigente da UEL\n";
    int opcao = numero("Papel: ");
    int secao = numero("ID da secao (0 para dirigente sem secao): ");
    if (!cin || opcao < 1 || opcao > 3 || secao < 0 || (opcao != 3 && secao == 0))
        return DadosUsuario();
    const string papeis[] = {"Jovem", "Chefe Escotista", "Dirigente da UEL"};
    return DadosUsuario(nome, nascimento, vinculo, papeis[opcao - 1], secao);
}

void menu() {
    Usuario dirigente(1, DadosUsuario("Dirigente da UEL", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoDirigente(dirigente);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    cout << "Azimute - cadastrar usuario.\n";
    while (cin) {
        cout << "\n1 Cadastrar usuario\n0 Sair\n";
        int opcao = numero("Opcao: ");
        if (!cin || opcao == 0) 
            return;
        if (opcao != 1) { cout << "Opcao invalida.\n"; 
            continue; }
        DadosUsuario dados = dadosUsuario();
        Usuario ana;
        if (!cin)   
            return;
        if (usuarios.registrar(dados, ana, idDirigenteUsuarios))
            cout << "Usuario cadastrado. ID: " << ana.id() << '\n';
        else 
            cout << "Cadastro recusado: dados invalidos.\n";
    }
}
