#include "Data.h"
#include <ctime>
using namespace std;

Data::Data() : ano_(0), mes_(0), dia_(0) {}

Data::Data(int ano, int mes, int dia) : ano_(ano), mes_(mes), dia_(dia) {}

Data::Data(const Data& o) : ano_(o.ano_), mes_(o.mes_), dia_(o.dia_) {}

Data& Data::operator=(const Data& o) {
    if (this != &o) {
        ano_ = o.ano_;
        mes_ = o.mes_;
        dia_ = o.dia_;
    }
    return *this;
}

Data::~Data() {}

int Data::year() const { return ano_; }

int Data::month() const { return mes_; }

int Data::day() const { return dia_; }

bool Data::valid() const { 
    return ano_ > 0 && mes_ >= 1 && mes_ <= 12 && dia_ >= 1 && dia_ <= 31; 
}

int Data::key() const { 
    return ano_ * 10000 + mes_ * 100 + dia_; 
}

bool Data::operator<=(const Data& o) const { 
    return key() <= o.key(); 
}

bool Data::operator==(const Data& o) const { 
    return key() == o.key(); 
}

Data Data::today() {
    time_t t = time(0);
    tm* agora = localtime(&t);
    return Data(agora->tm_year + 1900, agora->tm_mon + 1, agora->tm_mday);
}
