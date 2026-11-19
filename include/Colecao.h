#ifndef COLECAO_H
#define COLECAO_H
#include <vector>

using namespace std;

template <class T> class Colecao;

/**
  @brief Percorre uma Colecao, elemento por elemento, sem conhecer o domínio do cliente.
  Atributos: colecao_ (coleção que está sendo percorrida) e pos_ (posição do próximo elemento a ser devolvido).
 
  Uso típico: enquanto hasNext() for verdadeiro, chame next() para obter o próximo elemento.

  @tparam T Tipo dos elementos da coleção percorrida.
 */
template <class T>
class Iterador {
public:
    /**
      @brief Construtor padrão. Cria um iterador sem coleção associada (hasNext() devolve false).
     */
    Iterador();

    /**
      @brief Cria um iterador posicionado no primeiro elemento da coleção.
     
      @param colecao Coleção que será percorrida (deve continuar existindo durante a iteração).
     */
    explicit Iterador(const Colecao<T>* colecao);

    /**
      @brief Construtor de cópia.
     
      @param outro Iterador que será copiado (mantém a mesma coleção e a mesma posição).
     */
    Iterador(const Iterador& outro);

    /**
      @brief Atribui a coleção e a posição de outro iterador a este.
     
      @param outro Iterador cujos dados serão copiados.
      @return Referência para este iterador.
     */
    Iterador& operator=(const Iterador& outro);

    /**
         @brief Destrutor.
     */
    ~Iterador();

    /**
      @brief Verifica se ainda existem elementos a percorrer.
     
      @return true se há um próximo elemento; false se a coleção acabou ou não há coleção associada.
     */
    bool hasNext() const;

    /**
      @brief Devolve o elemento atual e avança para o próximo.
     
      @return Referência constante para o elemento atual.
      @throw std::out_of_range Se não houver mais elementos (chame hasNext() antes).
     */
    const T& next();

private:
    /** @brief Coleção que está sendo percorrida. */
    const Colecao<T>* colecao_;
    int pos_;
};

/**
  @brief Coleção genérica de elementos, que mantém a ordem de inserção e é acessada por posição.
  Atributo: itens_ (armazenamento interno dos elementos, na ordem de inserção).
 
  @tparam T Tipo dos elementos guardados (cada elemento é armazenado como cópia).
 */
template <class T>
class Colecao {
public:
    /**
      @brief Construtor padrão. Cria uma coleção vazia.
     */
    Colecao();

    /**
      @brief Construtor de cópia.
     
      @param outro Coleção cujos elementos serão copiados.
     */
    Colecao(const Colecao& outro);

    /**
      @brief Substitui o conteúdo desta coleção pelo de outra.
     
      @param outro Coleção cujos elementos serão copiados.
      @return Referência para esta coleção.
     */
    Colecao& operator=(const Colecao& outro);

    /**
      @brief Destrutor.
     */
    ~Colecao();

    /**
      @brief Adiciona um elemento ao final da coleção.
     
      @param item Elemento que será copiado para a coleção.
      @throw std::bad_alloc Se faltar memória para guardar o elemento.
     */
    void add(const T& item);

    /**
      @brief Substitui o elemento de uma posição existente.
     
      @param indice Posição (começando em 0) do elemento que será substituído.
      @param item Novo elemento que ocupará a posição.
      @throw std::out_of_range Se @p indice não for uma posição válida.
     */
    void set(int indice, const T& item);

    /**
      @brief Cria um iterador para percorrer a coleção do primeiro ao último elemento.
     
      @return Iterador posicionado no primeiro elemento.
     */
    Iterador<T> iterator() const;

    /**
      @brief Informa quantos elementos a coleção possui.
     
      @return Quantidade de elementos (0 se estiver vazia).
     */
    int size() const;

    /**
      @brief Obtém o elemento de uma posição.
     
      @param indice Posição (começando em 0) do elemento desejado.
      @return Referência constante para o elemento.
      @throw std::out_of_range Se @p indice não for uma posição válida.
     */
    const T& at(int indice) const;

private:
    vector<T> itens_;
};

#endif 
