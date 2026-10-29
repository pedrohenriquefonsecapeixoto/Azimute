#include "Data.h"
#include <ctime>
#include <limits>
#include <stdexcept>
#include <tuple>
using namespace std;

Data::Data() : ano_(0), mes_(0), dia_(0){}
Data::Data(int ano, int mes, int dia) : ano_(ano), mes_(mes), dia_(dia){}
Data::Data(const Data& outro) : ano_(outro.ano_), mes_(outro.mes_), dia_(outro.dia_){}

Data& Data::operator=(const Data& outro){
    if (this != &outro) {
        ano_ = outro.ano_;
        mes_ = outro.mes_;
        dia_ = outro.dia_;
    }
    return *this;
}

Data::~Data(){}

int Data::year() const{ 
    return ano_; 
}

int Data::month() const{ 
    return mes_; 
}

int Data::day() const{ 
    return dia_; 
}

bool Data::valid() const{ 
    if (ano_ <= 0 || mes_ < 1 || mes_ > 12 || dia_ < 1) 
        return false;
    const int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int limite = dias[mes_ - 1];
    if (mes_ == 2 && ano_ % 4 == 0 && (ano_ % 100 != 0 || ano_ % 400 == 0)) 
        ++limite;
    return dia_ <= limite; 
}

int Data::key() const { 
    long long valor = static_cast<long long>(ano_) * 10000 + static_cast<long long>(mes_) * 100 + dia_;
    if (valor < numeric_limits<int>::min() || valor > numeric_limits<int>::max())
        throw overflow_error("Data::key fora do intervalo de int");
    return static_cast<int>(valor); 
}

bool Data::operator<=(const Data& outro) const { 
    return tie(ano_, mes_, dia_) <= tie(outro.ano_, outro.mes_, outro.dia_); 
}

bool Data::operator==(const Data& outro) const { 
    return tie(ano_, mes_, dia_) == tie(outro.ano_, outro.mes_, outro.dia_); 
}

Data Data::today() {
    time_t instante = time(nullptr);
    tm* agora = localtime(&instante);
    return Data(agora->tm_year + 1900, agora->tm_mon + 1, agora->tm_mday);
}
