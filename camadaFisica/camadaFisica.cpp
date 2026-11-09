#include "camadaFisica.hpp"

int camada_fisica::binary_to_gray(int bit){
    return bit ^ (bit >> 1);
}

int camada_fisica::gray_to_binary(int bit){
    int mask = bit >> 1;
        while (mask != 0) {
            bit ^= mask;
            mask >>= 1;
        }
        return bit;
}

void camada_fisica::set_constelacao(){
    if(get_portadora() == PORTADORA){
        switch (get_modulacao()){
        case PSK8: {
            this->constelacao.resize(8);
            this->gray.resize(8);
            this->inv_gray.resize(8);

            for (int i = 0; i < 8; i++) {
                this->gray[i] = binary_to_gray(i);
                this->inv_gray[i] = gray_to_binary(i);
                double theta = gray[i] * (pi / 4.0);
                this->constelacao[inv_gray[i]] = {sin(theta), cos(theta)};
            }

            break;
        }
        case QAM32: {
            this->constelacao.resize(32);
            this->gray.resize(32);
            this->inv_gray.resize(32);

            for (int i = 0; i < 32; i++) {
                this->gray[i] = binary_to_gray(i);
                this->inv_gray[i] = gray_to_binary(i);
            }

            vector<simbolo_iq> mapa_constelacao;
            for (int i = -5; i <= 5; i += 2) {
                for (int q = -5; q <= 5; q += 2) {
                    if (abs(i) == 5 && abs(q) == 5) continue;
                    mapa_constelacao.push_back({(double) i, (double) q});
                }
            }

            double energia_total = 0;
            for (auto p : mapa_constelacao)
                energia_total += (p.fase * p.fase) + (p.quadratura * p.quadratura);

            double fator_escala = sqrt(32.0 / energia_total);

            for (int i = 0; i < 32; i++) {
                this->constelacao[inv_gray[i]] = {
                    mapa_constelacao[i].fase * fator_escala,
                    mapa_constelacao[i].quadratura * fator_escala
                };
            }
            break;
        }
        default:
            break;
        }
    }
}