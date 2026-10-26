#ifndef AZIMUTE_SRC_MODULOCONCRE_DATA_H
#define AZIMUTE_SRC_MODULOCONCRE_DATA_H

class Data {
public:
    Data();
    Data(int ano, int mes, int dia);
    Data(const Data& o);
    Data& operator=(const Data& o);
    ~Data();

    int year() const;
    int month() const;
    int day() const;

    bool valid() const;
    int key() const;
    bool operator<=(const Data& o) const;
    bool operator==(const Data& o) const;

    static Data today();

private:
    int ano_, mes_, dia_;
};

#endif 
