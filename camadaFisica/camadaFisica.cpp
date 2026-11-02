#include "camadaFisica.hpp"

bool camada_fisica::amostras_bit_valid(int val){
    if(val > 0 && val % 2 == 0)
        return true;
    return false;
}

void camada_fisica::set_modulacao(int cod_modulacao){
    this->cod_modulacao = cod_modulacao;
}

int camada_fisica::get_modulacao(){
    return this->cod_modulacao;
}

void camada_fisica::set_portadora(int cod_portadora){
    this->cod_portadora = cod_portadora;
}

int camada_fisica::get_portadora(){
    return this->cod_portadora;
}

void camada_fisica::set_cut_freq (double fc){
    this->cut_freq = fc;
}

double camada_fisica::get_cut_freq (){
    return this->cut_freq;
}

void camada_fisica::set_input_tension (double V){
    this->input_tension = V;
}

double camada_fisica::get_input_tension (){
    return this->input_tension;
}

void camada_fisica::set_amostragem(int amostragem){
    this->amostras_por_bit = amostragem;
}

int camada_fisica::get_amostragem(){
    return this->amostras_por_bit;
}
