#include "Menu.h"
#include <iostream>
#include <string>
#include "Usuario.h"
#include "Entrada.h"
#include "ServicoUsuarios.h"
#include "ServicoAtividades.h"
#include "ServicoPresenca.h"
#include "../../include/Colecao.h"
#include "Entidades.h"

using namespace std;

// Le uma linha de texto.
string texto(const char* pergunta) {
    cout << pergunta;
    string valor;
    getline(cin, valor);
    return valor;
}

// Se o usuario digitar algo invalido, devolve 0
// e quem chamou decide o que fazer 
int numero(const char* pergunta) {
    int valor = 0;
    cout << pergunta;
    cin >> valor;
    cin.clear();
    cin.ignore(1000, '\n'); // descarta o resto da linha
    return valor;
}

Data data() {
    int ano = numero("Ano: ");
    int mes = numero("Mes: ");
    int dia = numero("Dia: ");
    return Data(ano, mes, dia);
}

DadosUsuario dadosUsuario() {
    string nome = texto("Nome: ");
    cout << "Data de nascimento\n";
    Data nascimento = data();
    string vinculo = texto("Vinculo: ");
    cout << "1 Jovem\n2 Chefe Escotista\n3 Dirigente da UEL\n";
    int opcao = numero("Papel: ");
    int secao = numero("ID da secao (0 para dirigente sem secao): ");
    if (opcao < 1 || opcao > 3 || secao < 0 || (opcao != 3 && secao == 0))
        return DadosUsuario();
    const string papeis[] = {"Jovem", "Chefe Escotista", "Dirigente da UEL"};
    return DadosUsuario(nome, nascimento, vinculo, papeis[opcao - 1], secao);
}

void cadastrarAtividade(ServicoAtividades& atividades, int idChefeAtividades) {
    string titulo = texto("Titulo: ");
    Data dia = data();
    string horario = texto("Horario HH:MM: ");
    string local = texto("Local: ");
    string tipo = texto("Tipo: ");
    string ramo = texto("Ramo: ");
    Atividade reuniao;
    if (atividades.registrar(DadosAtividade(titulo, dia, horario, local, tipo, ramo), reuniao, idChefeAtividades))
        cout << "Atividade cadastrada. ID: " << reuniao.id() << '\n';
    else
        cout << "Cadastro recusado. Confira os dados e a permissao do usuario.\n";
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
    int idAtividade = numero("ID da atividade: ");
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
        int opcao = numero("1 Presente, 2 Ausente, 3 Atraso, 0 sem marcacao: ");
        if (opcao >= 1 && opcao <= 3)
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

void consultarHistorico(ServicoPresenca& presenca, int idDirigentePresenca) {
    int membro = numero("ID do jovem: ");
    cout << "Inicio do periodo\n";
    Data inicio = data();
    cout << "Fim do periodo\n";
    Data fim = data();
    if (!inicio.valid() || !fim.valid()) {
        cout << "Data invalida.\n";
        return;
    }
    FiltroHistorico filtro(inicio, fim, membro);
    Colecao<Participacao> historico = presenca.query(filtro, idDirigentePresenca);
    double percentual;
    if (!presenca.attendancePercentage(filtro, percentual, idDirigentePresenca)) {
        cout << "Sem registros no periodo ou consulta recusada.\n";
        return;
    }
    for (auto i = historico.iterator(); i.hasNext();) {
        const Participacao& registro = i.next();
        cout << "Atividade " << registro.activityId() << ": " << registro.status() << '\n';
    }
    cout << "Frequencia: " << percentual << "%\n";
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
    int idDirigentePresenca = presenca.add(sessaoDirigente);
    ServicoUsuarios usuarios;
    int idDirigenteUsuarios = usuarios.add(sessaoDirigente);
    cout << "Azimute - cadastrar usuario.\n";
    while (true) {
        cout << "\n1 Cadastrar usuario\n2 Consultar atividades\n3 Cadastrar atividade\n4 Registrar presenca\n5 Historico e frequencia\n0 Sair\n";
        int opcao = numero("Opcao: ");
        switch (opcao) {
            case 0:
                return;
            case 1: {
                DadosUsuario dados = dadosUsuario();
                Usuario ana;
                if (usuarios.registrar(dados, ana, idDirigenteUsuarios))
                    cout << "Usuario cadastrado. ID: " << ana.id() << '\n';
                else
                    cout << "Cadastro recusado: dados invalidos.\n";
                break;
            }
            case 2:
                listarAtividades(atividades, idChefeAtividades);
                break;
            case 3:
                cadastrarAtividade(atividades, idChefeAtividades);
                break;
            case 4:
                registrarPresenca(presenca, idChefePresenca);
                break;
            case 5:
                consultarHistorico(presenca, idDirigentePresenca);
                break;
            default:
                cout << "Opcao invalida.\n";
        }
    }
}
