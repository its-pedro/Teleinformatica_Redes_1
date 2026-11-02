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
        int amostras_por_bit = 0;
        double cut_freq = 0;
        double input_tension = 0;

        vector<int> bit_beacon;
        vector<double> modulated_signal;
        vector<double> modulated_port;
        
    public:
        int get_modulacao() {return this->cod_modulacao;}
        void set_modulacao(int cod_modulacao){this->cod_modulacao = cod_modulacao;}

        int get_portadora(){return this->cod_portadora;}
        void set_portadora(int cod_portadora){this->cod_portadora = cod_portadora;}

        double get_cut_freq(){return this->cut_freq;}
        void set_cut_freq(double fc){this->cut_freq = fc;}

        double get_input_tension(){return this->input_tension;}
        void set_input_tension(double V){this->input_tension = V;}

        int get_amostragem(){return this->amostras_por_bit;}
        void set_amostragem(int amostragem){this->amostras_por_bit = amostragem;}
};

class camada_fisica_transmissor: public camada_fisica{
    protected:
        void nrz_polar();
        void manchester();
        void bipolar();
    //modula_portadora()
    public:
        void modula_sinal();
        vector<double> get_modulated_signal() { return this->modulated_signal; }
};

class camada_fisica_receptor: public camada_fisica{
    //demodula_portadora()
    //demodula_banda_base()
};

#endif