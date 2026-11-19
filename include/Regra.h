#ifndef REGRA_H
#define REGRA_H

//regra que o cliente pode plugar nos servicos.
/**
 * @brief Define uma regra de validação que pode ser adicionada aos serviços.
 * @tparam T Tipo dos dados avaliados pela regra.
 */
template <class T>
class Regra {
public:
    /** @brief Inicializa a parte comum de uma regra. */
    Regra() {}

    /** @brief Permite destruir uma regra derivada pelo tipo base. */
    virtual ~Regra() {}

    /**
     * @brief Verifica se os dados atendem à regra.
     * @param dados Dados que serão avaliados.
     * @return true se a regra aceitar os dados; false se os rejeitar.
     */
    virtual bool allows(const T& dados) const = 0; // true = aceita
private:
    // Copia proibida: declarada e nunca implementada.
    Regra(const Regra&);
    Regra& operator=(const Regra&);
};

#endif