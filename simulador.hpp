#ifndef SIMULADOR_HPP
#define SIMULADOR_HPP

// Camada "de cola" entre a interface grafica e as camadas do trabalho
// (enlace + fisica). Nao depende de GTK, entao pode ser testada pelo terminal.

#include <random>
#include <string>
#include <vector>

#include "camadaEnlace.hpp"
#include "camadaFisica.hpp"

// Tudo que a pagina de controle/config permite ajustar
struct Config {
    int    enquadramento    = ENQ_CONTAGEM;
    int    edc              = EDC_NENHUM;
    int    tam_max_quadro   = 16;     // bytes de carga util por quadro
    int    tam_edc          = 8;      // bits (usado pelo checksum)
    int    modulacao        = 0b001;  // NRZ-polar / Manchester / Bipolar
    int    portadora        = 0;      // 0 = nenhuma; senao ASK/FSK/8PSK/32QAM
    int    amostras_por_bit = 100;
    double tensao           = 1.0;    // amplitude em volts
    double freq             = 2.0;    // ciclos por bit (cut_freq da camada fisica)
    double ruido            = 0.0;    // desvio padrao do ruido gaussiano do canal (V)
};

struct ResultadoTx {
    std::string                   texto;
    std::vector<int>              payload;       // bits da aplicacao
    std::vector<std::vector<int>> quadros;       // saida da camada de enlace
    std::vector<int>              bits;          // todos os quadros concatenados
    std::vector<double>           sinal_bb;      // banda base
    std::vector<double>           sinal_port;    // portadora (vazio se nao houver)
    bool                          usa_portadora = false;
    int                           amostras_por_bit = 0;   // valor efetivamente usado (sempre par)
    std::vector<std::string>      avisos;

    // sinal que realmente vai para o canal
    const std::vector<double>& sinal_transmitido() const {
        return usa_portadora ? sinal_port : sinal_bb;
    }
};

struct ResultadoRx {
    bool                          implementado = false;  // true quando o receptor existir
    std::vector<int>              bits;                  // bits recuperados da camada fisica
    std::vector<std::vector<int>> quadros;               // quadros recuperados pelo enlace
    std::string                   mensagem;              // texto entregue a aplicacao
    std::vector<std::string>      avisos;                // ex.: "quadro 2: erro detectado"
};

// A camada fisica guarda os bits em um membro protegido (bit_beacon) e nao
// tem setter. Esta subclasse so expoe um, sem mexer no seu camadaFisica.hpp.
class TxFisicaAcessivel : public camada_fisica_transmissor {
public:
    void define_bits(const std::vector<int>& bits) { bit_beacon = bits; }
};

class Simulador {
public:
    Simulador();

    static std::vector<int> texto_para_bits(const std::string& texto);
    static std::string      bits_para_texto(const std::vector<int>& bits);
    // "01001000 01101001 ..." (agrupa em bytes quando agrupa=true)
    static std::string      formata_bits(const std::vector<int>& bits, bool agrupa = true);

    ResultadoTx transmitir(const std::string& texto, const Config& cfg);

    // Canal: soma ruido gaussiano de desvio padrao 'sigma' (0 = canal ideal)
    std::vector<double> canal(const std::vector<double>& sinal, double sigma);

    // Gancho do receptor. Hoje devolve implementado=false; e aqui que entram
    // camada_fisica_receptor (demodulacao) e camada_enlace_receptor.
    ResultadoRx receber(const ResultadoTx& tx,
                        const std::vector<double>& sinal_recebido,
                        const Config& cfg);

private:
    camada_enlace_transmissor enlace_tx_;
    TxFisicaAcessivel         fisica_tx_;
    std::mt19937              rng_;
};

#endif
