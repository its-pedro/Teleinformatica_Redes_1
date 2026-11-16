#ifndef TESTE_CAMADA_FISICA_HPP
#define TESTE_CAMADA_FISICA_HPP

#include <iostream>
#include <vector>
#include <cassert>
#include "camadaFisica.hpp"

class teste_camada_fisica {
    protected:
        camada_fisica_transmissor *tx;
        camada_fisica_receptor *rx;

        vector<int> bit_stream = {0,1,0,1,0,0,1,1};
    public:
        teste_camada_fisica(){}
        void testa_nrz_polar();
        void testa_manchester();
        void testa_bipolar();

        void testa_ask();
        void testa_fsk();
        void testa_8psk();
        void testa_32qam();

        void run();
};

#endif