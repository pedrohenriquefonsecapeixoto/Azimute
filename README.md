# Azimute

Sistema de Gestão da Participação em Atividades da Unidade Escoteira Local (UEL).

## Sobre o projeto

O Azimute é um projeto de aplicativo móvel voltado à organização das atividades e ao acompanhamento da participação dos integrantes de uma Unidade Escoteira Local, também denominada Grupo Escoteiro.


## Objetivo

Aumentar o engajamento dos integrantes nas reuniões, atividades e eventos da UEL e reduzir o tempo gasto pelas lideranças com o registro e a consulta de frequência.


## Serviços previstos

| Serviço | Funcionalidades |
| --- | --- |
| Autenticação | Entrada no sistema, recuperação de senha e encerramento da sessão. |
| Controle de Acesso | Restrição de serviços e funções conforme o papel do usuário autenticado. |
| Perfil de Usuário | Consulta do próprio perfil, edição do nome de tratamento e definição de patrulha/equipe/matilha pelo Chefe Escotista. |
| Eventos e Atividades | Cadastro, consulta, atualização e remoção de reuniões, atividades e eventos. |
| Presença e Assiduidade | Registro e correção de presença, ausência e atraso, consulta ao histórico e tratamento de justificativas. |
| Materiais da Atividade | Administração da lista e das quantidades de materiais, solicitação de itens e resposta com disponibilidade e quantidade. |
| Usuários | Cadastro e manutenção dos dados dos integrantes da UEL. |
| Manutenção | Backup e restauração dos dados pelo Administrador. |


## API da lógica de negócio (Sprint 2)

Esta sprint implementa em C++ a API descrita no documento "Azimute – Projeto da API (Sprint 2)". O login e o cadastro de conta ficam para o final do projeto; por isso a API não tem serviço de autenticação e identifica quem pede pela sessão (`idSessao`).

| Serviço | Operações | Quem usa |
| --- | --- | --- |
| `ServicoUsuarios` | `registrar` | Dirigente da UEL |
| `ServicoAtividades` | `registrar`, `query` | Chefe Escotista |
| `ServicoPresenca` | `listMembers`, `record`, `recordingTime` | Chefe Escotista |
| `ServicoPresenca` | `query(FiltroHistorico)`, `attendancePercentage` | Jovem (só o próprio), Chefe (a própria seção), Dirigente (qualquer Jovem) |
| `ServicoPresenca` | `generatePendingAlerts`, `query(FiltroAlerta)` | Sistema (rotina interna) e Chefe |

Como usar, em resumo:
- A aplicação cria uma `Sessao` para o `Usuario` e chama `servico.add(sessao)`, que devolve o `idSessao`.
- Cada operação recebe o `idSessao`. O próprio serviço descobre o papel e a seção de quem pede e aplica as regras de acesso.
- Operações que podem falhar devolvem `bool` (`false` = nada foi criado ou alterado). Consultas devolvem uma `Colecao` (vazia se negado ou sem resultado).
- Regras do cliente (`Regra<T>`) podem ser plugadas com `servico.add(regra)`, sem mexer no framework.
- Os dados ficam em memória, no `Repositorio` (singleton). Os ids novos começam em 100.


## Estrutura do projeto

| Pasta ou arquivo | Conteúdo |
| --- | --- |
| `include/` | Tipos genéricos do framework: `Colecao<T>` (com `Iterador<T>`), `Regra<T>` e `Consulta<T, F>`. |
| `src/main.cpp` | Ponto de entrada: chama `menu()`. |
| `src/moduloConcre/` | Implementação: serviços (`ServicoUsuarios`, `ServicoAtividades`, `ServicoPresenca`), `Servico`, `Sessao`/`Usuario`, entidades (`Atividade`, `Participacao`, `Justificativa`, `Alerta`), `Data`, `Repositorio` e o menu de console. |
| `test/funcional/` | Testes funcionais, feitos com `assert`. |
| `bin/` | Executáveis gerados (não vão para o repositório). |
| `.gitlab-ci.yml` | Pipeline que compila com `-Werror` e roda os testes. |


## Como compilar e executar

Requisitos: `g++` com suporte a C++17 e `make`.

| Comando | O que faz |
| --- | --- |
| `make build` | Compila a aplicação em `bin/azimute`. |
| `make run` | Compila e executa a aplicação (menu de console com os três atores). |
| `make build_func` | Compila os testes funcionais em `bin/teste_funcional`. |
| `make run_func` | Compila e roda os testes funcionais. |
| `make clean` | Remove os executáveis gerados. |


## Testes funcionais

Os testes funcionais seguem os casos de uso do documento da API (cenários CT-01 e CT-02 de cada um e as alternativas dos fluxos) e usam `assert`. Se todos passarem, o programa termina com `FUNCIONAL: OK`. Se algum falhar, o programa aborta e o `assert` informa arquivo, linha, função e a mensagem do critério.

Regras de escrita dos testes:
- Chamada que altera estado (`registrar`, `record`, `remove`, `generatePendingAlerts`, `recordingTime`, `attendancePercentage`, `next` do iterador) é guardada em variável antes do `assert`. Dentro do `assert` ficam só leituras puras.
- Cada condição tem o seu `assert`, no formato `assert(condicao && "mensagem")`.
- Além do retorno (`true`/`false`), o teste confere o efeito no `Repositorio`: o que foi gravado, ou que nada foi criado.
- O arquivo faz `#undef NDEBUG` antes de incluir `<cassert>`, para que compilar com `-DNDEBUG` não desligue os testes.

Os testes estão organizados por caso de uso, na mesma ordem das issues:

| Caso de uso | Issue |
| --- | --- |
| Cadastrar usuário | #3 |
| Cadastrar atividade/evento | #4 |
| Consultar atividade/evento | #5 |
| Registrar presença, ausência ou atraso | #6 |
| Consultar histórico de participação | #7 |
| Alertar sobre justificativa pendente | #8 |

Os testes que usam a atividade de 03/10/2026 dependem do relógio do computador estar depois dessa data (a atividade precisa ter terminado).


## Limites do produto

Não faz parte do escopo:

- Inteligência artificial ou rede social.

O sistema apoia a organização das informações e não substitui as decisões dos chefes escotistas e dirigentes.
