#ifndef CAMADA_FISICA_HPP
#define CAMADA_FISICA_HPP

#include<stdexcept>

using namespace std;

// Prover modulacao banda-base
// NRZ-polar    001;
// Manchester   010;
// Bipolar      100;

// Prover codificacao de portadora
// ASK           0001;
// FSK           0010;
// PSK (8PSK)    0100;
// 32 QAM        1000;

class camada_fisica{
    protected:
        int code_modulacao = 0;
        int code_portadora = 0;

    public:
        int get_modulacao();
        void set_modulacao(int);

        int get_portadora();
        void set_portadora(int);
};

class camada_fisica_receptor: public camada_fisica{
    //demodula_portadora()
    //demodula_banda_base()
};

class camada_fisica_transmissor: public camada_fisica{
    //modula_portadora()
    //modula_banda_base()
};

#endif