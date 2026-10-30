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

Participacao::Participacao(int idIntegrante, const string& situacao)
    : idAtividade_(0), idIntegrante_(idIntegrante), situacao_(situacao), registradoEm_(0) {}

Participacao::Participacao(const Participacao& outro)
    : idAtividade_(outro.idAtividade_), idIntegrante_(outro.idIntegrante_),
      situacao_(outro.situacao_), registradoEm_(outro.registradoEm_){}

Participacao& Participacao::operator=(const Participacao& outro){
    if (this != &outro) {
        idAtividade_ = outro.idAtividade_;
        idIntegrante_ = outro.idIntegrante_;
        situacao_ = outro.situacao_;
        registradoEm_ = outro.registradoEm_;
    }
    return *this;
}

Participacao::~Participacao(){}

int Participacao::memberId() const{ 
    return idIntegrante_; }

string Participacao::status() const { 
    return situacao_; 
}

int Participacao::activityId() const { 
    return idAtividade_; 
}

time_t Participacao::recordedAt() const { 
    return registradoEm_; 
}

void Participacao::attach(int idAtividade, time_t quando) {
    idAtividade_ = idAtividade;
    registradoEm_ = quando;
}