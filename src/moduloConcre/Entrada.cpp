#include "Entrada.h"

using namespace std;

DadosUsuario::DadosUsuario() : idSecao_(0) {}

DadosUsuario::DadosUsuario(const string& nome, const Data& nascimento, const string& vinculo,
             const string& papel, int idSecao)
    : nome_(nome), nasc_(nascimento), vinculo_(vinculo), papel_(papel), idSecao_(idSecao) {}

DadosUsuario::DadosUsuario(const DadosUsuario& outro)
    : nome_(outro.nome_), nasc_(outro.nasc_), vinculo_(outro.vinculo_), papel_(outro.papel_), idSecao_(outro.idSecao_) {}

DadosUsuario& DadosUsuario::operator=(const DadosUsuario& outro) {
    if (this != &outro) {
        nome_ = outro.nome_;
        nasc_ = outro.nasc_;
        vinculo_ = outro.vinculo_;
        papel_ = outro.papel_;
        idSecao_ = outro.idSecao_;
    }
    return *this;
}

DadosUsuario::~DadosUsuario() {}

string DadosUsuario::name() const { 
    return nome_; 
}

Data DadosUsuario::birth() const {
    return nasc_; 
    }

string DadosUsuario::bond() const { 
    return vinculo_; 
}

string DadosUsuario::role() const { 
    return papel_; 
}

int DadosUsuario::sectionId() const { 
    return idSecao_; 
}

