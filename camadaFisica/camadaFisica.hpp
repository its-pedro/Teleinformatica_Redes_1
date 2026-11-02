#ifndef CAMADA_FISICA_HPP
#define CAMADA_FISICA_HPP

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// Prover modulacao banda-base
// NRZ-polar    0b001;
// Manchester   0b010;
// Bipolar      0b100;

// Prover codificacao de portadora
// ASK           0b0001;
// FSK           0b0010;
// PSK (8PSK)    0b0100;
// 32 QAM        0b1000;

class camada_fisica{
    protected:
        int cod_modulacao = 0;
        int cod_portadora = 0;
        double cut_freq = 0;
        double input_tension = 0;

        vector<int> bit_beacon;
        vector<int> modulated_signal;
        vector<int> modulated_port;

    public:
        bool modulacao_valid(int);
        bool portadora_valid(int);

        int get_modulacao();
        void set_modulacao(int);
        int get_portadora();
        void set_portadora(int);
        double get_cut_freq();
        void set_cut_freq(double);
        double get_input_tension();
        void set_input_tension(double);
};

class camada_fisica_transmissor: public camada_fisica{
    //modula_portadora()
    //modula_banda_base()
     
};

class camada_fisica_receptor: public camada_fisica{
    //demodula_portadora()
    //demodula_banda_base()
};

#endif