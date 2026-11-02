#include "camadaFisica.hpp"

void camada_fisica_transmissor::modula_sinal(){
    switch (this->cod_modulacao){
    case 0b001:
        nrz_polar();
        break;
    
    case 0b010:
        manchester();
        break;

    case 0b100:
        bipolar();
        break;

    default:
        break;
    }
}

void camada_fisica_transmissor::nrz_polar(){
    this->modulated_signal.clear();

    for(int bit : bit_beacon){
        double tension = (bit == 1)? this->input_tension : -(this->input_tension);

        for(int i = 0; i < this->amostras_por_bit; i++){
            this->modulated_signal.push_back(tension);
        }
    }
}

void camada_fisica_transmissor::manchester(){
    this->modulated_signal.clear();

    for(int bit : bit_beacon){
        for(int i = 0; i < this->amostras_por_bit/2; i++){
            int val = bit ^ 0;
            this->modulated_signal.push_back(val*this->input_tension);
        }

        for(int i = this->amostras_por_bit/2; i < this->amostras_por_bit; i++){
            int val = bit ^ 1;
            this->modulated_signal.push_back(val*this->input_tension);
        }
    }
}

void camada_fisica_transmissor::bipolar(){
    this->modulated_signal.clear();
    double tension;
    double ones_last = -input_tension;

    for(int bit : bit_beacon){
        tension = 0;
        
        if(bit == 1) {
            tension = -ones_last;
            ones_last= tension;
        }

        for(int i = 0; i < this->amostras_por_bit; i++){
            this->modulated_signal.push_back(tension);
        }
    }
}