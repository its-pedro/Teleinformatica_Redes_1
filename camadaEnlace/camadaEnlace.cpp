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


    /* 
    cabeçalho:
    O transmissor deverá encapsular a carga útil (payload) em quadros, 
    e o receptor deverá extraí-los corretamente. 
    */

// transmissor coloca 8 bits informando quantos bytes existem no conteúdo do quadro
vector<int> camada_enlace_transmissor::enquadra_contagem(const vector<int>& conteudo) const{
    vector<int> quadro = inteiro_para_bits(conteudo.size() / 8, 8); //representar a quantidade de bytes com conteúdo em 1 byte
    quadro.insert(quadro.end(), conteudo.begin(), conteudo.end()); //coloca o conteudo após o cabeçalho
    return quadro;
}
 

//paridade por coluna
vector<int> camada_enlace_transmissor::enquadra_entrelacamento(const vector<int>& conteudo) const{
    long long n = conteudo.size() / 8; //bytes de conteudo
    vector<int> quadro = inteiro_para_bits(n, 8);
 
    for(size_t col = 0; col < 8; col++){
        long long uns =0;
        for(size_t lin = 0; lin < n; lin++){
            uns += conteudo[lin * 8 + col];
        }
        quadro.push_back(uns % 2);
    }
    return quadro;
}
 
// FLAG [conteudo com ESC antes de FLAG/ESC] FLAG
// FLAG_BYTE para indicar o início e o fim de um quadro
vector<int> camada_enlace_transmissor::enquadra_byte_stuffing(const vector<int>& conteudo) const{
    vector<int> quadro = inteiro_para_bits(FLAG_BYTE, 8); //flag no inicio
 
    for(size_t i = 0; i + 8 <= conteudo.size(); i += 8){
        int byte = (int) bits_para_inteiro(conteudo, i, 8);
        if(byte == FLAG_BYTE || byte == ESC_BYTE){
            vector<int> esc = inteiro_para_bits(ESC_BYTE, 8);
            quadro.insert(quadro.end(), esc.begin(), esc.end()); //se tem um byte esc ou flag no conteudo, add mais um, pra saber que nao é fim
        }
        quadro.insert(quadro.end(), conteudo.begin() + i, conteudo.begin() + i + 8); //add o byte 
    }
 
    vector<int> flag = inteiro_para_bits(FLAG_BYTE, 8);
    quadro.insert(quadro.end(), flag.begin(), flag.end()); //coloca flag no final
    return quadro;
}
 
// FLAG [conteudo com um 0 apos cada cinco 1s consecutivos] FLAG
/*
como a flag tem seis uns consecutivos, se o conteudo tiver mts uns pode ser
confundido com uma flag
*/
vector<int> camada_enlace_transmissor::enquadra_bit_stuffing(const vector<int>& conteudo) const{
    vector<int> quadro = inteiro_para_bits(FLAG_BYTE, 8); //flag inicial
    int uns = 0;
 
    for(int bit : conteudo){
        quadro.push_back(bit);
        uns = (bit == 1) ? uns + 1 : 0;
        if(uns == 5){
            quadro.push_back(0); //zero apos 5 bits 2 consecutivos
            uns = 0;
        }
    }
 
    vector<int> flag = inteiro_para_bits(FLAG_BYTE, 8);
    quadro.insert(quadro.end(), flag.begin(), flag.end());
    return quadro;
}

//edc so serve pra detecção
//hamming: redundância q permite corrigir

// Posicoes 1..7: p1 p2 d1 p3 d2 d3 d4
// p1 cobre 3,5,7 | p2 cobre 3,6,7 | p3 cobre 5,6,7
vector<int> camada_enlace_transmissor::codifica_hamming(const vector<int>& bits) const{
    vector<int> saida;
    int p=0, r = 0;

    while((1 << r) < bits.size() + r + 1) r++;

    int n = bits.size() + r;
 
    for(int i = 1; i <= n; i++){
        if((i & (i-1)) == 0) saida.push_back(0);
        else{ 
            saida.push_back(bits[p]);
            p++;
        }
    }

    for(int i = 1; i <= n; i *= 2){
        for(int j = 0; j < n; j++){
            if((j+1) & i) saida[i-1] ^= saida[j];
        }
    }
    
    return saida;
}