#ifndef DATA_H
#define DATA_H

/**
  @brief Representa uma data do calendário (ano, mês e dia).
  Atributos: ano_ (ano), mes_ (mês) e dia_ (dia).
 
  A classe apenas guarda os valores; usa valid() para saber se a data existe de fato no calendário.
 */
class Data {
public:
    /**
       @brief Construtor padrão. Cria uma data zerada (0/0/0), que é considerada inválida por valid().
     */
    Data();

    /**
      @brief Cria uma data a partir de ano, mês e dia.
     
      Os valores não são validados aqui; use valid() para conferir.
     
      @param ano Ano da data (deve ser maior que zero para ser válida).
      @param mes Mês da data, de 1 (janeiro) a 12 (dezembro).
      @param dia Dia do mês, de 1 até o último dia do mês informado.
     */
    Data(int ano, int mes, int dia);

    /**
      @brief Construtor de cópia.
     
      @param outro Data que será copiada.
     */
    Data(const Data& outro);

    /**
      @brief Atribui os valores de outra data a esta.
     
      @param outro Data cujos valores serão copiados.
      @return Referência para esta data, permitindo atribuições encadeadas.
     */
    Data& operator=(const Data& outro);

    /**
      @brief Destrutor.
     */
    ~Data();

    /**
      @brief Informa o ano da data.
      @return Ano da data.
     */
    int year() const;

    /**
      @brief Informa o mês da data.
      @return Mês da data, de 1 a 12 (quando a data é válida).
     */
    int month() const;

    /**
      @brief Informa o dia da data.
      @return Dia do mês (quando a data é válida).
     */
    int day() const;

    /**
      @brief Verifica se a data existe no calendário.
     
      Considera o número de dias de cada mês e os anos bissextos (fevereiro com 29 dias).
     
      @return true se ano, mês e dia formam uma data real; false caso contrário.
     */
    bool valid() const;

    /**
      @brief Compara cronologicamente: verifica se esta data é anterior ou igual a outra.
     
      @param outro Data com a qual será comparada.
      @return true se esta data vem antes de @p outro ou é a mesma data.
     */
    bool operator<=(const Data& outro) const;

    /**
      @brief Verifica se duas datas são iguais (mesmo ano, mês e dia).
     
      @param outro Data com a qual será comparada.
      @return true se as duas datas forem iguais.
     */
    bool operator==(const Data& outro) const;

    /**
      @brief Obtém a data atual, segundo o relógio do sistema.
     
      @return Data de hoje.
     */
    static Data today();

private:
    int ano_;
    int mes_;
    int dia_;
};
#endif
