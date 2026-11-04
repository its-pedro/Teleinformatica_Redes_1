#include "camadaEnlace.hpp"
#include <cmath>

// dados sao vectors de bits

vector<int> camada_enlace::inteiro_para_bits(unsigned long long valor, int n_bits){
    vector<int> bits;
    // Inicia no MSB pq construo com push_back
    for(int i=n_bits-1; i>=0; i--){
        bits.push_back((valor >> i) & 1);      
    }
    return bits;
}

unsigned long long camada_enlace::bits_para_inteiro(const vector<int>& bits, size_t ini, size_t n_bits){
    unsigned long long valor = 0;
    
    for(size_t i = 0; i < n_bits; i++){
        valor += bits[ini+i] * pow(2, n_bits-1-i);
    }

    return valor;
}

int camada_enlace::busca_flag(const vector<int>& bits, size_t ini){
    for(size_t i =ini; i + 8<=bits.size(); i++){
        if(bits_para_inteiro(bits, i, 8) == (unsigned long long)FLAG_BYTE){
            return (int)i;
        }
    }
    return -1;
}

//quantos bits serão utilizados para armazenar o código
int camada_enlace::bits_do_edc() const{
    switch(this->cod_edc){
        case EDC_PARIDADE: return 8;  // 1 bit util, alinhado em 1 byte
        case EDC_CHECKSUM: return this->tamanho_edc;
        case EDC_CRC32:    return 32;
        default:           return 0;      
    }
}

vector<int> camada_enlace::calcula_edc(const vector<int>& dados) const{
    switch(this->cod_edc){
        case EDC_PARIDADE: return edc_paridade(dados);
        case EDC_CHECKSUM: return edc_checksum(dados);
        case EDC_CRC32:    return edc_crc32(dados);
        default:           return vector<int>();
    }
}

// recebe o vetor de bits
// Guardado no ultimo bit de um byte(vector de bits) para manter o quadro alinhado em bytes
vector<int> camada_enlace::edc_paridade(const vector<int>& dados) const{
    int uns =0;
    for(int b : dados) uns += b;
    if(uns % 2) return inteiro_para_bits(1, 8);
    else return inteiro_para_bits(0, 8);
}

// Checksum: soma de palavras de 'tamanho_edc' e inverte bits (receptor vai somar tudo e ver se da zero)
vector<int> camada_enlace::edc_checksum(const vector<int>& dados) const{
    const int w = this->tamanho_edc;
    unsigned long long soma = 0;
    unsigned long long palavra;
    int bit;

    for(size_t i=0; i <dados.size(); i+=w){ // percorre por w bits
        palavra = 0;
        for(int j = 0; j < w; j++){ //percorre cada bit
            
            if(i + j < dados.size()) bit = dados[ i+j ];
            else bit = 0;

            palavra*=2; palavra+= bit;
        }
        soma += palavra;
       
    }

    return inteiro_para_bits( (~soma) & ((1ULL << w )-1), w);
}

/*
// CRC-32 (polinomio gerador IEEE 802: 0x04C11DB7), divisao polinomial bit a bit.
// Sem inicializacao com 1s e sem reflexao: e a divisao "pura" do polinomio.
vector<int> camada_enlace::edc_crc32(const vector<int>& dados) const{
    const unsigned int polinomio = 0x04C11DB7; //polinomio gerador
    unsigned int reg = 0; // 32 bits que armazena result
    unsigned int topo; //deve-se aplicar polinomio?

    for(int bit : dados){
        // topo indica se msb do reg e cur bit sao diferentes
        topo = ((reg >> 31) & 1) ^ (bit & 1);
        reg <<= 1; // shift right com 0 no reg
        if(topo) reg ^= polinomio; // se forem diferentes fazemos xor
    }

    return inteiro_para_bits(reg, 32);
}
    */
