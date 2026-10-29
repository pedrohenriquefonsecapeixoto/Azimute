#include "Entidades.h"
using namespace std;

Atividade::Atividade() : id_(0), idSecao_(0){}
Atividade::Atividade(int id, const DadosAtividade& dados, int idSecao, const string& situacao)
    : id_(id), dados_(dados), idSecao_(idSecao), situacao_(situacao){}
Atividade::Atividade(const Atividade& outro)
    : id_(outro.id_), dados_(outro.dados_), idSecao_(outro.idSecao_), situacao_(outro.situacao_),
      integrantes_(outro.integrantes_){}
Atividade& Atividade::operator=(const Atividade& outro){
    if (this != &outro) {
        id_ = outro.id_;
        dados_ = outro.dados_;
        idSecao_ = outro.idSecao_;
        situacao_ = outro.situacao_;
        integrantes_ = outro.integrantes_;
    }
    return *this;
}

Atividade::~Atividade(){}

int Atividade::id() const { 
    return id_; 
}
const DadosAtividade& Atividade::details() const{ 
    return dados_; 
}

int Atividade::sectionId() const{ 
    return idSecao_; 
}

const vector<int>& Atividade::members() const{ 
    return integrantes_; 
}

void Atividade::enroll(int idIntegrante){ 
    integrantes_.push_back(idIntegrante); 
}

string Atividade::status() const{ 
    return situacao_; 
}

