#ifndef CONSULTA_H
#define CONSULTA_H

template <class T> class Colecao;

// Contrato de consulta: T = tipo do resultado, F = tipo do filtro.
/**
 * @brief Define a interface para consultas que usam um filtro e uma sessão.
 * @tparam T Tipo dos elementos devolvidos pela consulta.
 * @tparam F Tipo do filtro usado na consulta.
 */
template <class T, class F>
class Consulta {
public:
    /** @brief Inicializa a parte comum de uma consulta. */
    Consulta() {}

    /** @brief Permite destruir uma implementação derivada pelo tipo base. */
    virtual ~Consulta() {}

    /**
     * @brief Consulta os elementos de acordo com o filtro e a sessão informados.
     * @param filtro Critérios usados para selecionar os resultados.
     * @param idSessao Identificador da sessão de quem solicita a consulta.
     * @return Coleção de elementos do tipo T encontrados pela consulta.
     */
    virtual Colecao<T> query(const F& filtro, int idSessao) = 0;
private:
    Consulta(const Consulta&);
    Consulta& operator=(const Consulta&);
};

#endif