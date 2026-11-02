#include "camadaFisica.hpp"

void camada_fisica::set_modulacao(int code_modulacao){
    this->code_modulacao = code_modulacao;
}

int camada_fisica::get_modulacao(){
    return this->code_modulacao;
}

void camada_fisica::set_portadora(int code_portadora){
    this->code_portadora = code_portadora;
}

int camada_fisica::get_portadora(){
    return this->code_portadora;
}