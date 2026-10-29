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

DadosAtividade::DadosAtividade() {}

DadosAtividade::DadosAtividade(const string& titulo, const Data& data, const string& horario,
               const string& local, const string& tipo, const string& ramo,
               const string& descricao)
    : titulo_(titulo), data_(data), horario_(horario), local_(local),
      tipo_(tipo), ramo_(ramo), descricao_(descricao) {}

DadosAtividade::DadosAtividade(const DadosAtividade& outro)
    : titulo_(outro.titulo_), data_(outro.data_), horario_(outro.horario_), local_(outro.local_),
      tipo_(outro.tipo_), ramo_(outro.ramo_), descricao_(outro.descricao_) {}

DadosAtividade& DadosAtividade::operator=(const DadosAtividade& outro) {
    if (this != &outro) {
        titulo_ = outro.titulo_;
        data_ = outro.data_;
        horario_ = outro.horario_;
        local_ = outro.local_;
        tipo_ = outro.tipo_;
        ramo_ = outro.ramo_;
        descricao_ = outro.descricao_;
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

bool DadosAtividade::validStartTime() const {
    return horario_.size() == 5 && horario_[2] == ':' &&
        horario_[0] >= '0' && horario_[0] <= '2' &&
        horario_[1] >= '0' && horario_[1] <= '9' &&
        (horario_[0] != '2' || horario_[1] <= '3') &&
        horario_[3] >= '0' && horario_[3] <= '5' &&
        horario_[4] >= '0' && horario_[4] <= '9';
}

bool DadosAtividade::endTime(int duracaoMin, time_t& fim) const {
    if (!data_.valid() || !validStartTime() || duracaoMin < 0) 
        return false;
    tm inicio = tm();
    inicio.tm_year = data_.year() - 1900;
    inicio.tm_mon = data_.month() - 1;
    inicio.tm_mday = data_.day();
    inicio.tm_hour = (horario_[0] - '0') * 10 + horario_[1] - '0';
    inicio.tm_min = (horario_[3] - '0') * 10 + horario_[4] - '0';
    inicio.tm_isdst = -1;
    time_t quando = mktime(&inicio);
    if (quando == time_t(-1)) 
        return false;
    // Rejeita horarios locais inexistentes, normalizados pelo mktime.
    if (inicio.tm_year != data_.year() - 1900 || inicio.tm_mon != data_.month() - 1 ||
        inicio.tm_mday != data_.day() ||
        inicio.tm_hour != (horario_[0] - '0') * 10 + horario_[1] - '0' ||
        inicio.tm_min != (horario_[3] - '0') * 10 + horario_[4] - '0') 
            return false;
    fim = quando + static_cast<time_t>(duracaoMin) * 60;
    return true;
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