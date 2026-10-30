#include "Menu.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include "Usuario.h"
#include "Entrada.h"
#include "ServicoUsuarios.h"
#include "ServicoAtividades.h"
#include "ServicoPresenca.h"
#include "../../include/Colecao.h"
#include "Entidades.h"

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

void cadastrarAtividade(ServicoAtividades& atividades, int idChefeAtividades) {
    string titulo = texto("Titulo: ");
    Data dia = data();
    if (!cin) return;
    string horario;
    while (cin) {
        horario = texto("Horario HH:MM: ");
        if (!cin) return;
        if (DadosAtividade("", dia, horario, "", "", "").validStartTime()) break;
        cout << "Horario invalido. Informe HH:MM, de 00:00 a 23:59.\n";
    }
    string local = texto("Local: ");
    string tipo = texto("Tipo: ");
    string ramo = texto("Ramo: ");
    if (!cin) return;
    Atividade reuniao;
    if (atividades.registrar(DadosAtividade(titulo, dia, horario, local, tipo, ramo), reuniao, idChefeAtividades))
        cout << "Atividade cadastrada. ID: " << reuniao.id() << '\n';
    else cout << "Cadastro recusado. Confira os dados e a permissao do usuario.\n";
}

void listarAtividades(ServicoAtividades& atividades, int idChefeAtividades) {
    auto lista = atividades.query(FiltroAtividade(Data(1, 1, 1), Data(9999, 12, 31)), idChefeAtividades);
    cout << lista.size() << " atividade(s)\n";
    for (auto i = lista.iterator(); i.hasNext();) {
        const Atividade& a = i.next();
        Data d = a.details().date();
        cout << a.id() << " - " << a.details().title() << " - "
             << d.day() << '/' << d.month() << '/' << d.year() << ' '
             << a.details().startTime() << '\n';
    }
}

void registrarPresenca(ServicoPresenca& presenca, int idChefePresenca) {
    int idAtividade = numero("ID da atividade: ", 1);
    if (!cin) 
        return;
    Colecao<Usuario> integrantes = presenca.listMembers(idAtividade, idChefePresenca);
    if (integrantes.size() == 0) {
        cout << "Atividade nao encontrada, de outra secao ou sem integrantes inscritos.\n";
        return;
    }
    const string situacoes[] = {"Presente", "Ausente", "Atraso"};
    Colecao<Participacao> marcacoes;
    for (auto i = integrantes.iterator(); i.hasNext();) {
        const Usuario& jovem = i.next();
        cout << jovem.id() << " - " << jovem.details().name() << '\n';
        int opcao = numero("1 Presente, 2 Ausente, 3 Atraso, 0 sem marcacao: ", 0, 3);
        if (!cin) return;
        if (opcao != 0)
            marcacoes.add(Participacao(jovem.id(), situacoes[opcao - 1]));
    }
    if (marcacoes.size() == 0) {
        cout << "Nenhuma marcacao informada.\n";
        return;
    }
    if (!presenca.record(idAtividade, marcacoes, idChefePresenca)) {
        cout << "Registro recusado: a atividade precisa ter terminado (inicio + 120 min), "
                "nao estar cancelada e os integrantes nao podem ter registro anterior.\n";
        return;
    }
    cout << "Presenca registrada.\n";
    int minutos;
    if (presenca.recordingTime(idAtividade, minutos, idChefePresenca))
        cout << "Tempo entre o fim da atividade e o registro: " << minutos << " min\n";
}

void menu() {
    Usuario dirigente(1, DadosUsuario("Dirigente da UEL", Data(1980, 1, 1), "UEL", "Dirigente da UEL", 0));
    Sessao sessaoDirigente(dirigente);
    Usuario carlos(2, DadosUsuario("Chefe Escotista", Data(1980, 1, 1), "UEL", "Chefe Escotista", 2));
    Sessao sessaoChefe(carlos);
    ServicoAtividades atividades;
    int idChefeAtividades = atividades.add(sessaoChefe);
    ServicoPresenca presenca;
    int idChefePresenca = presenca.add(sessaoChefe);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    cout << "Azimute - cadastrar usuario.\n";
    while (cin) {
        cout << "\n1 Cadastrar usuario\n2 Consultar atividades\n3 Cadastrar atividade\n4 Registrar presenca\n0 Sair\n";
        int opcao = numero("Opcao: ");
        if (!cin || opcao == 0) 
            return;
        if(opcao == 4){
            registrarPresenca(presenca, idChefePresenca);
            continue;
        }    
        if (opcao == 3) {
            cadastrarAtividade(atividades, idChefeAtividades);
            continue;
        }
        if (opcao == 2) {
            listarAtividades(atividades, idChefeAtividades);
            continue;
        }
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
