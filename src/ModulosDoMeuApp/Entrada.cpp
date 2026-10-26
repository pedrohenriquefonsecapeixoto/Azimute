#include "Entrada.h"
using namespace std;

DadosUsuario::DadosUsuario() : idSecao_(0) {}

DadosUsuario::DadosUsuario(const string& nome, const Data& nascimento, const string& vinculo,const string& papel, int idSecao) : nome_(nome), nasc_(nascimento), vinculo_(vinculo), papel_(papel), idSecao_(idSecao) {}

DadosUsuario::DadosUsuario(const DadosUsuario& o) : nome_(o.nome_), nasc_(o.nasc_), vinculo_(o.vinculo_), papel_(o.papel_), idSecao_(o.idSecao_) {}

DadosUsuario& DadosUsuario::operator=(const DadosUsuario& o) {
    if (this != &o) {
        nome_ = o.nome_;
        nasc_ = o.nasc_;
        vinculo_ = o.vinculo_;
        papel_ = o.papel_;
        idSecao_ = o.idSecao_;
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

DadosAtividade::DadosAtividade() {}

DadosAtividade::DadosAtividade(const string& titulo, const Data& data, const string& horario, const string& local, const string& tipo, const string& ramo, const string& descricao) : titulo_(titulo), data_(data), horario_(horario), local_(local), tipo_(tipo), ramo_(ramo), descricao_(descricao) {}

DadosAtividade::DadosAtividade(const DadosAtividade& o) : titulo_(o.titulo_), data_(o.data_), horario_(o.horario_), local_(o.local_), tipo_(o.tipo_), ramo_(o.ramo_), descricao_(o.descricao_) {}

DadosAtividade& DadosAtividade::operator=(const DadosAtividade& o) {
    if (this != &o) {
        titulo_ = o.titulo_;
        data_ = o.data_;
        horario_ = o.horario_;
        local_ = o.local_;
        tipo_ = o.tipo_;
        ramo_ = o.ramo_;
        descricao_ = o.descricao_;
    }
    return *this;
}

DadosAtividade::~DadosAtividade() {}

string DadosAtividade::title() const { 
    return titulo_; 
}

Data DadosAtividade::date() const { 
    return data_; 
}

string DadosAtividade::startTime() const { 
    return horario_; 
}

string DadosAtividade::place() const { 
    return local_; 
}

string DadosAtividade::type() const { 
    return tipo_; 
}

string DadosAtividade::branch() const { 
    return ramo_; 
}

string DadosAtividade::description() const { 
    return descricao_; 
}

FiltroAtividade::FiltroAtividade(const Data& inicio, const Data& fim, const string& situacao, int idSecao) : inicio_(inicio), fim_(fim), situacao_(situacao), idSecao_(idSecao) {}

FiltroAtividade::FiltroAtividade(const FiltroAtividade& o) : inicio_(o.inicio_), fim_(o.fim_), situacao_(o.situacao_), idSecao_(o.idSecao_) {}

FiltroAtividade& FiltroAtividade::operator=(const FiltroAtividade& o) {
    if (this != &o) {
        inicio_ = o.inicio_;
        fim_ = o.fim_;
        situacao_ = o.situacao_;
        idSecao_ = o.idSecao_;
    }
    return *this;
}

FiltroAtividade::~FiltroAtividade() {}

Data FiltroAtividade::start() const { 
    return inicio_; 
}

Data FiltroAtividade::end() const { 
    return fim_; 
}

string FiltroAtividade::status() const { 
    return situacao_; 
}

int FiltroAtividade::sectionId() const { 
    return idSecao_; 
}

FiltroHistorico::FiltroHistorico(const Data& inicio, const Data& fim, int idIntegrante, int idSecao) : inicio_(inicio), fim_(fim), idIntegrante_(idIntegrante), idSecao_(idSecao) {}

FiltroHistorico::FiltroHistorico(const FiltroHistorico& o) : inicio_(o.inicio_), fim_(o.fim_), idIntegrante_(o.idIntegrante_), idSecao_(o.idSecao_) {}

FiltroHistorico& FiltroHistorico::operator=(const FiltroHistorico& o) {
    if (this != &o) {
        inicio_ = o.inicio_;
        fim_ = o.fim_;
        idIntegrante_ = o.idIntegrante_;
        idSecao_ = o.idSecao_;
    }
    return *this;
}

FiltroHistorico::~FiltroHistorico() {}

Data FiltroHistorico::start() const { 
    return inicio_; 
}

Data FiltroHistorico::end() const { 
    return fim_; 
}

int FiltroHistorico::memberId() const { 
    return idIntegrante_; 
} 

int FiltroHistorico::sectionId() const { 
    return idSecao_; 
}

FiltroAlerta::FiltroAlerta() {}

FiltroAlerta::FiltroAlerta(const FiltroAlerta&) {}

FiltroAlerta& FiltroAlerta::operator=(const FiltroAlerta&) { 
    return *this; 
}

FiltroAlerta::~FiltroAlerta() {}
