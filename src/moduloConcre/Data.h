#ifndef DATA_H
#define DATA_H

class Data {
public:
    Data();
    Data(int ano, int mes, int dia);
    Data(const Data& outro);
    Data& operator=(const Data& outro);
    ~Data();
    int year() const;
    int month() const;
    int day() const;
    bool valid() const;
    bool operator<=(const Data& outro) const;
    bool operator==(const Data& outro) const;
    static Data today();
private:
    int ano_, mes_, dia_;
};
#endif 