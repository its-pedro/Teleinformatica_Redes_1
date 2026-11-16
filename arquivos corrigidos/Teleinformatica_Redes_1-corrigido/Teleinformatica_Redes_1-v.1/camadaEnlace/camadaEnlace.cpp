#include "camadaEnlace.hpp"
#include <cmath>

// dados sao vectors de bits
const vector<int> gerador = {1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};

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
    
    for(size_t i = 0; i < n_bits; i++)
        valor = (valor << 1) | bits[ini + i];
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


// CRC-32 (polinomio gerador IEEE 802: 0x04C11DB7), divisao polinomial bit a bit.
// Sem inicializacao com 1s e sem reflexao: e a divisao "pura" do polinomio.
vector<int> camada_enlace::edc_crc32(const vector<int>& dados) const{
    vector<int> aux = dados;
    vector<int> zero_pad(gerador.size()-1, 0);
    
    aux.insert(aux.end(), zero_pad.begin(), zero_pad.end());

    for(int i = 0; i < aux.size() - zero_pad.size(); i++)
        if(aux[i] == 1)
            for(int j = 0; j < gerador.size(); j++)
                aux[i+j] ^= gerador[j];

    vector<int> crc;
    for(int i = dados.size(); i < aux.size(); i++)
        crc.push_back(aux[i]);

    return crc;
} 


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
    quadro.insert(quadro.end(), conteudo.begin(), conteudo.end()); 
 
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


vector<vector<int>> camada_enlace_transmissor::processa(const vector<int>& payload) const{
    vector<vector<int>> quadros;
 
    // O contador de bytes tem 8 bits, entao conteudo (carga + EDC) <= 255 bytes
    int max_bytes = min(this->tamanho_max_quadro, 255 - bits_do_edc() / 8);
    if(max_bytes < 1) max_bytes = 1;
    size_t max_bits = (size_t) max_bytes * 8;
 
    for(size_t i = 0; i < payload.size(); i += max_bits){
        size_t fim = min(i + max_bits, payload.size());
        vector<int> conteudo(payload.begin() + i, payload.begin() + fim);
 
        // 1) Anexa o EDC (calculado sobre a carga util)
        vector<int> edc = calcula_edc(conteudo);
        conteudo.insert(conteudo.end(), edc.begin(), edc.end());
 
        // 2) Enquadramento
        vector<int> quadro;
        switch(this->cod_enquadramento){
            case ENQ_CONTAGEM:       quadro = enquadra_contagem(conteudo); break;
            case ENQ_ENTRELACAMENTO: quadro = enquadra_entrelacamento(conteudo); break;
            case ENQ_BYTE_STUFFING:  quadro = enquadra_byte_stuffing(conteudo); break;
            case ENQ_BIT_STUFFING:   quadro = enquadra_bit_stuffing(conteudo); break;
            default:                 quadro = conteudo; break;
        }
 
        // 3) Correcao de erros (sobre o quadro inteiro, protegendo ate os delimitadores)
        if(this->cod_edc == EDC_HAMMING){
            quadro = codifica_hamming(quadro);
        }
 
        quadros.push_back(quadro);
    }
    return quadros;
}

//desenquadramento

bool camada_enlace_receptor::desenquadra_contagem(const vector<int>& bits, vector<int>& conteudo) const{
    //primeiros 8 bits informam tamanho da msg
    if(bits.size() < 8) return false;
 
    size_t n_bits = bits_para_inteiro(bits, 0, 8) * 8;
    if(bits.size() < 8 + n_bits) return false;      
 
    conteudo.assign(bits.begin() + 8, bits.begin() + 8 + n_bits);
    return true;
}
 
bool camada_enlace_receptor::desenquadra_entrelacamento(const vector<int>& bits, vector<int>& conteudo) const {
    if(bits.size() < 8) return false;

    size_t n = bits_para_inteiro(bits, 0, 8);

    if(bits.size() < 8 + n * 8 + 8) return false;

    conteudo.assign(bits.begin() + 8, bits.begin() + 8 + n * 8);

    for(size_t col = 0; col < 8; col++){
        int uns = 0;

        for(size_t lin = 0; lin < n; lin++){
            uns += conteudo[lin * 8 + col];
        }

        if(uns % 2 != bits[8 + n * 8 + col]){
            return false;
        }
    }

    return true;
}
 
bool camada_enlace_receptor::desenquadra_byte_stuffing(const vector<int>& bits, vector<int>& conteudo) const{
    size_t n_bytes = bits.size() / 8;
    if(n_bytes < 2) return false;
    if((int) bits_para_inteiro(bits, 0, 8) != FLAG_BYTE) return false;
 
    conteudo.clear();
    for(size_t i = 1; i < n_bytes; i++){
        int byte = (int) bits_para_inteiro(bits, i * 8, 8);
 
        if(byte == FLAG_BYTE){
            return true;                                 // flag final
        }
        if(byte == ESC_BYTE){
            if(++i >= n_bytes) return false;             // ESC sem byte seguinte
            byte = (int) bits_para_inteiro(bits, i * 8, 8);
        }
        vector<int> b = inteiro_para_bits(byte, 8);
        conteudo.insert(conteudo.end(), b.begin(), b.end());
    }
    return false;                                        // nao achou flag final
}
 
bool camada_enlace_receptor::desenquadra_bit_stuffing(const vector<int>& bits, vector<int>& conteudo) const{
    int ini = busca_flag(bits, 0);
    if(ini < 0) return false;
 
    int fim = busca_flag(bits, ini + 8);
    if(fim < 0) return false;
 
    conteudo.clear();
    int uns = 0;
    for(int i = ini + 8; i < fim; i++){
        if(uns == 5){  // 0 inserido é descartado
            if(bits[i] != 0) return false;
            uns = 0;
            continue;
        }
        conteudo.push_back(bits[i]);
        uns = (bits[i] == 1) ? uns + 1 : 0;
    }
    return true;
}


vector<int> camada_enlace_receptor::decodifica_hamming(const vector<int>& bits, int& corrigidos) const {
    vector<int> saida;
    corrigidos = 0;

    int n = bits.size();
    int r = 0;

    while((1 << r) < n + 1) r++;

    if((1 << r) - r - 1 <= 0) return saida;

    vector<int> c(bits);

    int sindrome = 0;

    for(int i = 1; i <= n; i *= 2){
        int paridade = 0;

        for(int j = 1; j <= n; j++){
            if(j & i) paridade ^= c[j-1];
        }

        if(paridade) sindrome += i;
    }

    if(sindrome != 0){
        if(sindrome > n) return {};

        c[sindrome - 1] ^= 1;
        corrigidos++;
    }

    for(int i = 1; i <= n; i++){
        if((i & (i-1)) != 0){
            saida.push_back(c[i-1]);
        }
    }

    return saida;
}

bool camada_enlace_receptor::decodifica_crc32(const vector<int>& bits) const{
    vector<int> aux = bits;


    for(int i = 0; i <= aux.size() - gerador.size(); i++){
        if(aux[i] == 1)
            for(int j = 0; j < gerador.size(); j++)
                aux[i+j] ^= gerador[j];
    }
        
    bool ok = true;
    for(int i = aux.size()-1; i > aux.size() - gerador.size(); i--){
        if(aux[i] != 0) ok = false;
    }
    return ok;
}

// ---- Pipeline do receptor -------------------------------------------------

quadro_recebido camada_enlace_receptor::processa_quadro(const vector<int>& quadro) const{
    quadro_recebido resultado;
    vector<int> bits = quadro;

    // 1) Correcao de erros
    if(this->cod_edc == EDC_HAMMING){
        bits = decodifica_hamming(bits, resultado.bits_corrigidos);
        if(bits.empty()){
        resultado.diagnostico = "Quadro descartado: erro de Hamming";
        return resultado;
    }
    }

    // 2) Desenquadramento
    vector<int> conteudo;
    bool ok = false;
    switch(this->cod_enquadramento){
        case ENQ_CONTAGEM:       ok = desenquadra_contagem(bits, conteudo); break;
        case ENQ_ENTRELACAMENTO: ok = desenquadra_entrelacamento(bits, conteudo); break;
        case ENQ_BYTE_STUFFING:  ok = desenquadra_byte_stuffing(bits, conteudo); break;
        case ENQ_BIT_STUFFING:   ok = desenquadra_bit_stuffing(bits, conteudo); break;
        default:                 conteudo = bits; ok = true; break;
    }
    if(!ok){
        resultado.diagnostico = "Quadro descartado: erro de enquadramento";
        return resultado;
    }

    // 3) Verificacao do EDC
    int n_edc = bits_do_edc();
    if(n_edc > 0){
        if((int) conteudo.size() < n_edc){
            resultado.diagnostico = "Quadro descartado: menor que o EDC";
            return resultado;
        }
        vector<int> carga(conteudo.begin(), conteudo.end() - n_edc);
        vector<int> edc_recebido(conteudo.end() - n_edc, conteudo.end());

        if(this->cod_edc == EDC_CRC32){
            if(!decodifica_crc32(conteudo)){
                resultado.diagnostico  = "Quadro descartado: EDC invalido (erro detectado)";
                return resultado;
            }
        }else{
            if(calcula_edc(carga) != edc_recebido){
                resultado.diagnostico = "Quadro descartado: EDC invalido (erro detectado)";
                return resultado;
            }
        }
        conteudo = carga;
    }

    resultado.valido = true;
    resultado.payload = conteudo;
    resultado.diagnostico = (resultado.bits_corrigidos > 0)
        ? "Quadro OK (" + to_string(resultado.bits_corrigidos) + " bit(s) corrigido(s))"
        : "Quadro OK";
    return resultado;
}

vector<quadro_recebido> camada_enlace_receptor::processa(const vector<vector<int>>& quadros) const{
    vector<quadro_recebido> resultados;
    for(const vector<int>& quadro : quadros){
        resultados.push_back(processa_quadro(quadro));
    }
    return resultados;
}

vector<int> camada_enlace_receptor::remonta_mensagem(const vector<quadro_recebido>& quadros){
    vector<int> mensagem;
    for(const quadro_recebido& q : quadros){
        if(q.valido){
            mensagem.insert(mensagem.end(), q.payload.begin(), q.payload.end());
        }
    }
    return mensagem;
}