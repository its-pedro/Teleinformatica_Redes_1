#ifndef CAMADA_ENLACE_HPP
#define CAMADA_ENLACE_HPP

#include <vector>

using namespace std;

const int ENQ_CONTAGEM      = 0b0001;   // Contagem de caracteres
const int ENQ_ENTRELACAMENTO= 0b0010;   // Entrelacamento de bits
const int ENQ_BYTE_STUFFING = 0b0100;   // Flags + insercao de bytes
const int ENQ_BIT_STUFFING  = 0b1000;   // Flags + insercao de bits

const int EDC_NENHUM    = 0b0000;
const int EDC_PARIDADE  = 0b0001;       
const int EDC_CHECKSUM  = 0b0010;      
const int EDC_CRC32     = 0b0100;      
const int EDC_HAMMING   = 0b1000;   
const int FLAG_BYTE = 0x7E;    
const int ESC_BYTE  = 0x7D;  



class camada_enlace {
    protected:
        
        int cod_enquadramento = ENQ_CONTAGEM;
        int cod_edc = EDC_NENHUM;
        int tamanho_max_quadro = 16;  
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

        int get_enquadramento() {return this->cod_enquadramento;}
        void set_enquadramento(int cod) {this->cod_enquadramento = cod;}
 
        int get_edc() {return this->cod_edc;}
        void set_edc(int cod) {this->cod_edc = cod;}
 
        int get_tamanho_max_quadro() {return this->tamanho_max_quadro;}
        void set_tamanho_max_quadro(int bytes) {this->tamanho_max_quadro = bytes;}
 
        int get_tamanho_edc() {return this->tamanho_edc;}
        void set_tamanho_edc(int bits) {this->tamanho_edc = bits;}
};

//transmissor
class camada_enlace_transmissor: public camada_enlace {
    private:
        // Enquadramento
        vector<int> enquadra_contagem(const vector<int>& conteudo) const;
        vector<int> enquadra_entrelacamento(const vector<int>& conteudo) const;
        vector<int> enquadra_byte_stuffing(const vector<int>& conteudo) const;
        vector<int> enquadra_bit_stuffing(const vector<int>& conteudo) const;
 
        // Correcao de erros
        vector<int> codifica_hamming(const vector<int>& bits) const;
 
    public:
        // Recebe os bits da aplicacao e devolve a lista de quadros prontos
        // para a camada fisica
        vector<vector<int>> processa(const vector<int>& payload) const;
};


#endif