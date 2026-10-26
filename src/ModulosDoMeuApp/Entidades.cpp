#include "Entidades.h"
using namespace std;

Atividade::Atividade() : id_(0), idSecao_(0) {}

Atividade::Atividade(int id, const DadosAtividade& dados, int idSecao, const string& situacao) : id_(id), dados_(dados), idSecao_(idSecao), situacao_(situacao) {}

Atividade::Atividade(const Atividade& o) : id_(o.id_), dados_(o.dados_), idSecao_(o.idSecao_), situacao_(o.situacao_) {}

Atividade& Atividade::operator=(const Atividade& o) {
    if (this != &o) {
        id_ = o.id_;
        dados_ = o.dados_;
        idSecao_ = o.idSecao_;
        situacao_ = o.situacao_;
    }
    return *this;
}

Atividade::~Atividade() {}

int Atividade::id() const { 
    return id_; 
}

const DadosAtividade& Atividade::details() const { 
    return dados_; 
}

int Atividade::sectionId() const { 
    return idSecao_; 
}

string Atividade::status() const { 
    return situacao_;
}

Participacao::Participacao(int idIntegrante, const string& situacao)
    : idAtividade_(0), idIntegrante_(idIntegrante), situacao_(situacao), registradoEm_(0) {}

Participacao::Participacao(const Participacao& o)
    : idAtividade_(o.idAtividade_), idIntegrante_(o.idIntegrante_),
      situacao_(o.situacao_), registradoEm_(o.registradoEm_) {}

Participacao& Participacao::operator=(const Participacao& o) {
    if (this != &o) {
        idAtividade_ = o.idAtividade_;
        idIntegrante_ = o.idIntegrante_;
        situacao_ = o.situacao_;
        registradoEm_ = o.registradoEm_;
    }
    return *this;
}

Participacao::~Participacao() {}

int Participacao::memberId() const { 
    return idIntegrante_; 
}

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

Justificativa::Justificativa(int id, int idIntegrante, int idAtividade, int idSecao,
                       const string& estado)
    : id_(id), idIntegrante_(idIntegrante), idAtividade_(idAtividade),
      idSecao_(idSecao), estado_(estado) {}

Justificativa::Justificativa(const Justificativa& o)
    : id_(o.id_), idIntegrante_(o.idIntegrante_), idAtividade_(o.idAtividade_),
      idSecao_(o.idSecao_), estado_(o.estado_) {}

Justificativa& Justificativa::operator=(const Justificativa& o) {
    if (this != &o) {
        id_ = o.id_;
        idIntegrante_ = o.idIntegrante_;
        idAtividade_ = o.idAtividade_;
        idSecao_ = o.idSecao_;
        estado_ = o.estado_;
    }
    return *this;
}

Justificativa::~Justificativa() {}

int Justificativa::id() const { 
    return id_; 
}

int Justificativa::sectionId() const { 
    return idSecao_; 
}

string Justificativa::status() const { 
    return estado_; 
}

void Justificativa::setStatus(const string& estado) { 
    estado_ = estado; 
}

Alerta::Alerta(int idDestinatario, int idJustificativa)
    : idDest_(idDestinatario), idJust_(idJustificativa) {}

Alerta::Alerta(const Alerta& o) : idDest_(o.idDest_), idJust_(o.idJust_) {}

Alerta& Alerta::operator=(const Alerta& o) {
    if (this != &o) {
        idDest_ = o.idDest_;
        idJust_ = o.idJust_;
    }
    return *this;
}

Alerta::~Alerta() {}

int Alerta::recipientId() const { 
    return idDest_; 
}

int Alerta::justificationId() const { 
    return idJust_; 
}
