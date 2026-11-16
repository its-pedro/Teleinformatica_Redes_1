#include "camadaFisica.hpp"

// Converte um numero binario para seu codigo gray correspondente
int camada_fisica::binary_to_gray(int bit){
    return bit ^ (bit >> 1);
}

// Converte um numero codificado com para seu binario correspondente
int camada_fisica::gray_to_binary(int bit){
    int mask = bit >> 1;
        while (mask != 0) {
            bit ^= mask;
            mask >>= 1;
        }
        return bit;
}

// Rotina que constroi as constelacoes de simbolos eletricos que
// serao utilizadas caso utilize-se modulacao por portadora 8psk ou 32qam
void camada_fisica::set_constelacao(){
    if(get_portadora() == PORTADORA){
        switch (get_modulacao()){
        case PSK8: {

            // Neste modelo de constelacao apenas associamos, para
            // cada posicao ao longo do circulo um simbolo eletrico de 3 bits
            // modelado utilizando a codificacao gray (para diminuir o BER)
            this->constelacao.resize(8);
            this->gray.resize(8);
            this->inv_gray.resize(8);

            for (int i = 0; i < 8; i++) {
                this->gray[i] = binary_to_gray(i);
                this->inv_gray[i] = gray_to_binary(i);
                
                // Como os simbolos estao igualmente espacados o valor do simbolo iq
                // em tensao sera o seno (fase) ou cosseno (quadratura) do numero de gray
                // (que demarca posicao) vezes pi/4
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

            // Para modelar a constelacao 32 qam utiliza-se um grid de poontos
            // utilizando -5 -3 -1 +1 +3 +5 tanto na vertical quanto na horizonal
            // (discartando-se os extremos) e associa cada ponto no grid a um simbolo iq
            vector<simbolo_iq> mapa_constelacao;
            for (int i = -5; i <= 5; i += 2) {
                for (int q = -5; q <= 5; q += 2) {
                    if (abs(i) == 5 && abs(q) == 5) continue;
                    mapa_constelacao.push_back({(double) i, (double) q});
                }
            }

            // Calcula as distancias a cada um dos pontos utilizados no grid
            // e usa esse valor para calcular um fator de ajuste que torna o circulo
            // unitario e mantem a distribuicao dos pontos 
            double energia_total = 0;
            for (auto p : mapa_constelacao)
                energia_total += (p.fase * p.fase) + (p.quadratura * p.quadratura);

            double fator_escala = sqrt(32.0 / energia_total);

            // Para cada simbolo anteriormente armazenado na constelacao multiplica
            // fase e quadratura pelo fator de escala para obter o valor a ser utilizado
            // e armazena esse valor o indexando utilizando o codigo de gray da posicao  
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