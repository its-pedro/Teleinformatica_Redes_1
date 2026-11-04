#ifndef CAMADA_ENLACE_HPP
#define CAMADA_ENLACE_HPP

#include <vector>

using namespace std;


const int EDC_NENHUM    = 0b0000;
const int EDC_PARIDADE  = 0b0001;       
const int EDC_CHECKSUM  = 0b0010;      
const int EDC_CRC32     = 0b0100;      
const int EDC_HAMMING   = 0b1000;      

class camada_enlace {
    protected:
        
        int cod_edc = EDC_NENHUM;
        int tamanho_edc = 8;       

        static vector<int> inteiro_para_bits(unsigned long long valor, int n_bits);
        static unsigned long long bits_para_inteiro(const vector<int>& bits, size_t inicio, size_t n_bits);

        vector<int> calcula_edc(const vector<int>& dados) const;
        vector<int> edc_paridade(const vector<int>& dados) const;
        vector<int> edc_checksum(const vector<int>& dados) const;
        vector<int> edc_crc32(const vector<int>& dados) const;

        int bits_do_edc() const;

        // Busca o padrao de flag (01111110) a partir de 'inicio'; retorna -1 se nao achar
        static int busca_flag(const vector<int>& bits, size_t inicio);

    public:

        int get_edc() {return this->cod_edc;}
        void set_edc(int cod) {this->cod_edc = cod;}

        int get_tamanho_edc() {return this->tamanho_edc;}
        void set_tamanho_edc(int bits) {this->tamanho_edc = bits;}
};


#endif