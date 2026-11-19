#ifndef ENTRADA_H
#define ENTRADA_H
#include <string>
#include <ctime>
#include "Data.h"
using namespace std;

/**
 * @brief Dados de cadastro de um usuario (nome, nascimento, vinculo, papel e secao).
 * Objeto de valor usado como entrada para criar um Usuario.
 */
class DadosUsuario {
public:
    /** @brief Cria dados de usuario vazio, com idSecao igual a 0. */
    DadosUsuario();

    /**
     * @brief Cria dados de usuario com todos os campos informados.
     * @param nome Nome do usuario.
     * @param nascimento Data de nascimento do usuario.
     * @param vinculo Vinculo do usuario com o grupo escoteiro.
     * @param papel Papel do usuario no sistema (ex.: "Chefe Escotista" ou "Jovem").
     * @param idSecao Identificador da secao do usuario (0 = sem secao).
     */
    DadosUsuario(const string& nome, const Data& nascimento, const string& vinculo,
                 const string& papel, int idSecao = 0);

    /**
     * @brief Construtor de copia.
     * @param outro Objeto DadosUsuario a ser copiado.
     */             
    DadosUsuario(const DadosUsuario& outro);

    /**
     * @brief Operador de atribuicao.
     * @param outro Objeto DadosUsuario cujo conteudo sera copiado.
     * @return Referencia para o proprio objeto.
     */
    DadosUsuario& operator=(const DadosUsuario& outro);

    /** @brief Destrutor. */    
    ~DadosUsuario();

    /**
     * @brief Obtem o nome do usuario.
     * @return Nome do usuario.
     */
    string name() const;

    /**
     * @brief Obtem a data de nascimento do usuario.
     * @return Data de nascimento.
     */
    Data birth() const;

    /**
     * @brief Obtem o vinculo do usuario com o grupo.
     * @return Vinculo do usuario.
     */    
    string bond() const;

    /**
     * @brief Obtem o papel do usuario no sistema.
     * @return Papel do usuario (ex.: "Chefe Escotista" ou "Jovem").
     */    
    string role() const;

    /**
     * @brief Obtem o identificador da secao do usuario.
     * @return Id da secao (0 = sem secao).
     */
    int sectionId() const;

private:
    string nome_;
    Data nasc_;
    string vinculo_, papel_;
    int idSecao_;
};



/**
 * @brief Dados de uma atividade (titulo, data, horario, local, tipo, ramo e descricao).
 * Objeto de valor usado como entrada para registrar uma Atividade.
 */
class DadosAtividade {
public:
    /** @brief Cria dados de atividade vazios. */
    DadosAtividade();

    /**
     * @brief Cria dados de atividade com todos os campos.
     * @param titulo Titulo da atividade.
     * @param data Data em que a atividade ocorre.
     * @param horario Horario de inicio.
     * @param local Local onde a atividade acontece.
     * @param tipo Tipo da atividade.
     * @param ramo Ramo escoteiro ao qual a atividade se destina.
     * @param descricao Descricao da atividade (opcional).
     */    
    DadosAtividade(const string& titulo, const Data& data, const string& horario,
                   const string& local, const string& tipo, const string& ramo,
                   const string& descricao = "");

    /**
     * @brief Construtor de copia.
     * @param outro Objeto DadosAtividade a ser copiado.
     */                   
    DadosAtividade(const DadosAtividade& outro);

    /**
     * @brief Operador de atribuicao.
     * @param outro Objeto DadosAtividade cujo conteudo sera copiado.
     * @return Referencia para o proprio objeto (*this).
     */
    DadosAtividade& operator=(const DadosAtividade& outro);

    /** @brief Destrutor. */
    ~DadosAtividade();

    /**
     * @brief Obtem o titulo da atividade.
     * @return Titulo da atividade.
     */
    string title() const;

    /**
     * @brief Obtem a data da atividade.
     * @return Data da atividade.
     */
    Data date() const;

    /**
     * @brief Obtem o horario de inicio da atividade.
     * @return Horario de inicio.
     */
    string startTime() const;
    
    /**
     * @brief Verifica se o horario de inicio esta no formato valido.
     * @return true se o horario e valido, false caso contrario.
     */
    bool validStartTime() const;

    /**
     * @brief Calcula o instante de termino da atividade a partir da duracao.
     * @param duracaoMin Duracao da atividade em minutos.
     * @param fim Parametro que recebe o instante de termino.
     * @return true se a data e o horario sao validos e o termino foi calculado, false caso contrario.
     */    
    bool endTime(int duracaoMin, time_t& fim) const;


    /**
     * @brief Obtem o local da atividade.
     * @return Local da atividade.
     */    
    string place() const;

    /**
     * @brief Obtem o ramo escoteiro da atividade.
     * @return Ramo da atividade.
     */    
    string type() const;


    /**
     * @brief Obtem a descricao da atividade.
     * @return Descricao da atividade (pode ser vazia).
     */    
    string branch() const;

    /**
     * @brief Obtem a descricao da atividade.
     * @return Descricao da atividade (pode ser vazia).
     */    
    string description() const;

private:
    string titulo_;
    Data data_;
    string horario_, local_, tipo_, ramo_, descricao_;
};

/**
 * @brief Criterios de busca de atividades (periodo, situacao e secao).
 * Usado em ServicoAtividades::query.
 * idSecao igual a 0 significa "sem secao especifica".
 */
class FiltroAtividade {
public:
    /**
     * @brief Cria um filtro de atividades.
     * @param inicio Data inicial do periodo.
     * @param fim Data final do periodo.
     * @param situacao Situacao da atividade.
     * @param idSecao Identificador da secao.
     */
    explicit FiltroAtividade(const Data& inicio = Data(), const Data& fim = Data(),
                             const string& situacao = "", int idSecao = 0);

    /**
     * @brief Construtor de copia.
     * @param outro Filtro a ser copiado.
     */                         
    FiltroAtividade(const FiltroAtividade& outro);

    /**
     * @brief Operador de atribuicao.
     * @param outro Filtro cujo conteudo sera copiado.
     * @return Referencia para o proprio objeto.
     */    
    FiltroAtividade& operator=(const FiltroAtividade& outro);

    /** @brief Destrutor. */    
    ~FiltroAtividade();

    /**
     * @brief Obtem a data inicial do periodo.
     * @return Data inicial.
     */    
    Data start() const;

    /**
     * @brief Obtem a data final do periodo.
     * @return Data final.
     */    
    Data end() const;

    /**
     * @brief Obtem a situacao buscada.
     * @return Situacao da atividade.
     */    
    string status() const;

    /**
     * @brief Obtem o identificador da secao buscada.
     * @return Id da secao.
     */    
    int sectionId() const;

private:
    Data inicio_, fim_;
    string situacao_;
    int idSecao_;
};


/**
 * @brief Criterios de busca do historico de presenca (periodo, integrante e secao).
 * idIntegrante igual a 0 significa o proprio usuario (ou a secao inteira, se idSecao != 0).
 */
class FiltroHistorico {
public:
    /**
     * @brief Cria um filtro de historico.
     * @param inicio Data inicial do periodo buscado.
     * @param fim Data final do periodo buscado.
     * @param idIntegrante Identificador do integrante.
     * @param idSecao Identificador da secao.
     */
    explicit FiltroHistorico(const Data& inicio = Data(), const Data& fim = Data(),
                             int idIntegrante = 0, int idSecao = 0);

     /**
     * @brief Construtor de copia.
     * @param outro Filtro a ser copiado.
     */
    FiltroHistorico(const FiltroHistorico& outro);

    /**
     * @brief Operador de atribuicao.
     * @param outro Filtro cujo conteudo sera copiado.
     * @return Referencia para o proprio objeto.
     */    
    FiltroHistorico& operator=(const FiltroHistorico& outro);

    /** @brief Destrutor. */    
    ~FiltroHistorico();

    /**
     * @brief Obtem a data inicial do periodo.
     * @return Data inicial.
     */    
    Data start() const;

    /**
     * @brief Obtem a data final do periodo.
     * @return Data final.
     */    
    Data end() const;
    
    /**
     * @brief Obtem o identificador do integrante buscado.
     * @return Id do integrante.
     */    
    int memberId() const;

    /**
     * @brief Obtem o identificador da secao buscada.
     * @return Id da secao (0 = sem secao).
     */
    int sectionId() const;

private:
    Data inicio_, fim_;
    int idIntegrante_, idSecao_;
};

/**
 * @brief Filtro de alertas, sem criterios.
 * A Sessao ja delimita os alertas do usuario.
 */
class FiltroAlerta {
public:
    /** @brief Cria um filtro de alertas. */
    FiltroAlerta();

    /**
     * @brief Construtor de copia.
     * @param outro Filtro a ser copiado.
     */
    FiltroAlerta(const FiltroAlerta&);

    /**
     * @brief Operador de atribuicao.
     * @param outro Filtro a ser atribuido.
     * @return Referencia para o proprio objeto.
     */    
    FiltroAlerta& operator=(const FiltroAlerta&);
 
    /** @brief Destrutor. */    
    ~FiltroAlerta();
};

#endif // AZIMUTE_SRC_MODULOCONCRE_ENTRADA_H
