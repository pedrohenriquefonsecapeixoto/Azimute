#include "Usuario.h"

using namespace std;

Usuario::Usuario() : id_(0){}

Usuario::Usuario(int id, const DadosUsuario& dados) : id_(id), dados_(dados){}

Usuario::Usuario(const Usuario& outro) : id_(outro.id_), dados_(outro.dados_){}

Usuario& Usuario::operator=(const Usuario& outro){
    if (this != &outro) {
        id_ = outro.id_;
        dados_ = outro.dados_;
    }
    return *this;
}

Usuario::~Usuario(){}

int Usuario::id() const{ 
    return id_; 
}

const DadosUsuario& Usuario::details() const{ 
    return dados_; 
}

string Usuario::role() const{ 
    return dados_.role(); 
}

int Usuario::sectionId() const{ 
    return dados_.sectionId();
}

Sessao::Sessao(const Usuario& usuario) : usuario_(&usuario){}

Sessao::Sessao(const Sessao& outro) : usuario_(outro.usuario_){}

Sessao& Sessao::operator=(const Sessao& outro){
    if (this != &outro) usuario_ = outro.usuario_;
    return *this;
}

Sessao::~Sessao(){}

const Usuario& Sessao::user() const{ 
    return *usuario_; 
}
