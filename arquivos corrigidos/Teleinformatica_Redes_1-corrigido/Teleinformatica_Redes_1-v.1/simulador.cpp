#include "simulador.hpp"

#include <algorithm>
#include <exception>

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

    // Na camada fisica 'portadora' e so um seletor (BANDA_BASE ou PORTADORA);
    // o codigo da modulacao (NRZ/Manchester/Bipolar ou ASK/FSK/8PSK/32QAM)
    // vai em 'modulacao'.
    const bool port = cfg.portadora != 0;
    fisica_tx_.define_bits(r.bits);
    fisica_tx_.set_portadora(port ? PORTADORA : BANDA_BASE);
    fisica_tx_.set_modulacao(port ? cfg.portadora : cfg.modulacao);
    fisica_tx_.set_amostragem(apb);
    fisica_tx_.set_input_tension(cfg.tensao);
    fisica_tx_.set_cut_freq(cfg.freq);

    fisica_tx_.set_last_bipolar(-1);   // polaridade inicial do Bipolar a cada transmissao
    fisica_tx_.modula_sinal();
    if (port) {
        r.sinal_port    = fisica_tx_.get_modulated_signal();
        r.usa_portadora = !r.sinal_port.empty();
        if (!r.usa_portadora)
            r.avisos.push_back("A modulacao de portadora escolhida nao gerou sinal; "
                               "o canal vai transmitir o sinal vazio.");
    } else {
        r.sinal_bb = fisica_tx_.get_modulated_signal();
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

ResultadoRx Simulador::receber(const ResultadoTx& tx, const std::vector<double>& sinal_recebido,
                               const Config& cfg) {
    ResultadoRx r;
    r.implementado = true;

    if (sinal_recebido.empty()) {
        r.avisos.push_back("Sinal recebido vazio: nada para demodular.");
        return r;
    }

    // ---- camada fisica (demodulacao) ----
    const bool port = tx.usa_portadora;
    camada_fisica_receptor fisica_rx;
    fisica_rx.set_portadora(port ? PORTADORA : BANDA_BASE);
    fisica_rx.set_modulacao(port ? cfg.portadora : cfg.modulacao);
    fisica_rx.set_amostragem(tx.amostras_por_bit);
    fisica_rx.set_input_tension(cfg.tensao);
    fisica_rx.set_cut_freq(cfg.freq);
    fisica_rx.set_last_bipolar(-1);

    // as rotinas de banda base percorrem o sinal em blocos de 'apb' amostras
    std::vector<double> sinal = sinal_recebido;
    if (!port && tx.amostras_por_bit > 0 && sinal.size() % tx.amostras_por_bit)
        sinal.resize(sinal.size() - sinal.size() % tx.amostras_por_bit);

    try {
        fisica_rx.set_modulated_signal(sinal);
        fisica_rx.demodula_sinal();
        r.bits = fisica_rx.get_bit_beacon();
    } catch (const std::exception& e) {
        r.avisos.push_back(std::string("Falha na demodulacao: ") + e.what());
        return r;
    }

    // a portadora (8-PSK/32-QAM) pode devolver bits de preenchimento no final
    if (r.bits.size() > tx.bits.size()) r.bits.resize(tx.bits.size());
    if (r.bits.size() < tx.bits.size())
        r.avisos.push_back("A camada fisica recuperou menos bits do que foram transmitidos (" +
                           std::to_string(r.bits.size()) + " de " +
                           std::to_string(tx.bits.size()) + ").");

    // ---- camada de enlace ----
    // O canal e um fluxo continuo de bits; como o simulador conhece os limites
    // dos quadros enviados, usa os mesmos tamanhos para separar os quadros recebidos.
    size_t pos = 0;
    for (const auto& q : tx.quadros) {
        if (pos >= r.bits.size()) break;
        size_t fim = std::min(r.bits.size(), pos + q.size());
        r.quadros.emplace_back(r.bits.begin() + pos, r.bits.begin() + fim);
        pos = fim;
    }

    enlace_rx_.set_enquadramento(cfg.enquadramento);
    enlace_rx_.set_edc(cfg.edc);
    enlace_rx_.set_tamanho_max_quadro(cfg.tam_max_quadro);
    enlace_rx_.set_tamanho_edc(cfg.tam_edc);

    try {
        const auto resultado = enlace_rx_.processa(r.quadros);
        for (size_t i = 0; i < resultado.size(); i++) {
            const auto& q = resultado[i];
            std::string linha = "quadro " + std::to_string(i + 1) + ": " +
                                (q.valido ? "ok" : "ERRO");
            if (!q.diagnostico.empty()) linha += " - " + q.diagnostico;
            if (q.bits_corrigidos > 0)
                linha += " (" + std::to_string(q.bits_corrigidos) + " bit(s) corrigido(s))";
            r.avisos.push_back(linha);
        }
        r.mensagem = bits_para_texto(camada_enlace_receptor::remonta_mensagem(resultado));
    } catch (const std::exception& e) {
        r.avisos.push_back(std::string("Falha no enlace receptor: ") + e.what());
    }
    return r;
}
