#ifndef CAMADA_FISICA_HPP
#define CAMADA_FISICA_HPP

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>

const double pi = acos(-1);

using namespace std;

struct simbolo_iq{
    double fase;
    double quadratura;
};

// Escolher entre banda-base e portadora
// Banda-base    0b01;
// Portadora     0b10;

// Prover modulacao banda-base
// NRZ-polar     0b0001;
// Manchester    0b0010;
// Bipolar       0b0100;

// Prover codificacao de portadora
// ASK           0b0001;
// FSK           0b0010;
// PSK (8PSK)    0b0100;
// 32 QAM        0b1000;

class camada_fisica{
    protected:
        int portadora_banda_base = 0;
        int cod_modulacao = 0;
        int amostras_por_simbolo = 0;
        double cut_freq = 0;
        double input_tension = 0;
        int last_bipolar = -1;

        vector<int> bit_beacon;
        vector<double> modulated_signal;

        vector<simbolo_iq> constelacao;
        vector<int> gray;
        vector<int> inv_gray;

        int gray_to_binary(int);
        int binary_to_gray(int);

    public:
        void set_constelacao();

        vector<double> get_modulated_signal() {return this->modulated_signal;}
        void set_modulated_signal(vector<double> signal){this->modulated_signal = signal;}

        vector<int> get_bit_beacon() {return this->bit_beacon;}
        void set_bit_beacon(vector<int> beacon){this->bit_beacon = beacon;}

        int get_modulacao() {return this->cod_modulacao;}
        void set_modulacao(int cod_modulacao){this->cod_modulacao = cod_modulacao;}

        int get_portadora(){return this->portadora_banda_base;}
        void set_portadora(int cod){this->portadora_banda_base = cod;}

        double get_cut_freq(){return this->cut_freq;}
        void set_cut_freq(double fc){this->cut_freq = fc;}

        double get_input_tension(){return this->input_tension;}
        void set_input_tension(double V){this->input_tension = V;}

        int get_amostragem(){return this->amostras_por_simbolo;}
        void set_amostragem(int amostragem){this->amostras_por_simbolo = amostragem;}

        int get_last_bipolar() {return this->last_bipolar;}
        void set_last_bipolar(int last) {this->last_bipolar = last;}
};

class camada_fisica_transmissor: public camada_fisica{
    protected:
    //modulacao banda base
        void nrz_polar();
        void manchester();
        void bipolar();

    //modulacao da portadora
        void ask();
        void fsk();
        void psk8();
        void qam32();

    public:
        void modula_sinal();
};

class camada_fisica_receptor: public camada_fisica{
    protected:
    //demodula_portadora()
        void nrz_polar();
        void manchester();
        void bipolar();

    //demodula_banda_base()
        void ask();
        void fsk();
        void psk8();
        void qam32();

    public:
        void demodula_sinal();
};

#endif