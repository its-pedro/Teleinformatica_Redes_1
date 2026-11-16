#include "simulador.hpp"

Simulador::Simulador() : rng_(std::random_device{}()) {}

std::vector<int> Simulador::texto_para_bits(const std::string& texto) {
    std::vector<int> bits;
    for (unsigned char c : texto)
        for (int i = 7; i >= 0; i--) bits.push_back((c >> i) & 1);
    return bits;
}

std::string Simulador::bits_para_texto(const std::vector<int>& bits) {
    std::string s;
    for (size_t i = 0; i + 8 <= bits.size(); i += 8) {
        unsigned char c = 0;
        for (int j = 0; j < 8; j++) c = (c << 1) | (bits[i + j] & 1);
        s.push_back((char)c);
    }
    return s;
}

std::string Simulador::formata_bits(const std::vector<int>& bits, bool agrupa) {
    std::string s;
    for (size_t i = 0; i < bits.size(); i++) {
        if (agrupa && i > 0 && i % 8 == 0) s += ' ';
        s += bits[i] ? '1' : '0';
    }
    return s;
}

ResultadoTx Simulador::transmitir(const std::string& texto, const Config& cfg) {
    ResultadoTx r;
    r.texto   = texto;
    r.payload = texto_para_bits(texto);

    // ---- camada de enlace ----
    enlace_tx_.set_enquadramento(cfg.enquadramento);
    enlace_tx_.set_edc(cfg.edc);
    enlace_tx_.set_tamanho_max_quadro(cfg.tam_max_quadro);
    enlace_tx_.set_tamanho_edc(cfg.tam_edc);
    r.quadros = enlace_tx_.processa(r.payload);
    for (const auto& q : r.quadros) r.bits.insert(r.bits.end(), q.begin(), q.end());

    // ---- camada fisica ----
    // Manchester usa amostras_por_bit/2, entao forca um valor par
    int apb = cfg.amostras_por_bit;
    if (apb < 2) apb = 2;
    if (apb % 2) apb++;
    r.amostras_por_bit = apb;

    fisica_tx_.define_bits(r.bits);
    fisica_tx_.set_modulacao(cfg.modulacao);
    fisica_tx_.set_portadora(cfg.portadora);
    fisica_tx_.set_amostragem(apb);
    fisica_tx_.set_input_tension(cfg.tensao);
    fisica_tx_.set_cut_freq(cfg.freq);

    fisica_tx_.modula_sinal();
    r.sinal_bb = fisica_tx_.get_modulated_signal();

    if (cfg.portadora != 0) {
        fisica_tx_.modula_portadora();
        r.sinal_port    = fisica_tx_.get_portadora();
        r.usa_portadora = !r.sinal_port.empty();
        if (!r.usa_portadora)
            r.avisos.push_back("A modulacao de portadora escolhida ainda nao esta implementada "
                               "(sinal vazio); o canal vai transmitir o sinal em banda base.");
    }
    if (r.quadros.empty())
        r.avisos.push_back("A camada de enlace nao gerou nenhum quadro.");
    return r;
}

std::vector<double> Simulador::canal(const std::vector<double>& sinal, double sigma) {
    if (sigma <= 0) return sinal;
    std::normal_distribution<double> ruido(0.0, sigma);
    std::vector<double> saida(sinal.size());
    for (size_t i = 0; i < sinal.size(); i++) saida[i] = sinal[i] + ruido(rng_);
    return saida;
}

ResultadoRx Simulador::receber(const ResultadoTx&, const std::vector<double>&, const Config&) {
    ResultadoRx r;
    r.implementado = false;
    r.avisos.push_back("Receptor ainda nao implementado: faltam a demodulacao "
                       "(camada_fisica_receptor) e o desenquadramento + verificacao de EDC "
                       "(camada_enlace_receptor). Ligue-os em Simulador::receber().");
    // Quando existirem, o fluxo esperado e:
    //   r.bits     = demodula(sinal_recebido, cfg);
    //   r.quadros  = desenquadra(r.bits, cfg);          // + checagem do EDC
    //   r.mensagem = bits_para_texto(payload_recuperado);
    return r;
}
