#include "camadaFisica.hpp"

bool camada_fisica::modulacao_valid(int cod){
    if(cod != 0b001 || cod != 0b010 || cod != 0b100)
        return false;
    return true;
}

bool camada_fisica::portadora_valid(int cod){
if(cod != 0b0001 || cod != 0b0010 || cod != 0b0100 || cod != 0b1000)
        return false;
    return true;
}

void camada_fisica::set_modulacao(int cod_modulacao){
    if(modulacao_valid(cod_modulacao))
        this->cod_modulacao = cod_modulacao;
}

int camada_fisica::get_modulacao(){
    return this->cod_modulacao;
}

void camada_fisica::set_portadora(int cod_portadora){
    if(portadora_valid(cod_portadora))
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

double camada_fisica::get_cut_freq (){
    return this->input_tension;
}