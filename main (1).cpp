// Interface grafica (gtkmm 3) do simulador de camadas Fisica + Enlace.
// Tres abas: Controle/Configuracao, Transmissor (TX) e Receptor (RX).

#include <gtkmm.h>

#include <cstdio>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "grafico.hpp"
#include "simulador.hpp"

// ---------------------------------------------------------------- utilitarios

// codigos usados pelas camadas (mesma ordem dos itens das caixas de selecao)
static const int COD_ENQ[]  = {ENQ_CONTAGEM, ENQ_ENTRELACAMENTO, ENQ_BYTE_STUFFING, ENQ_BIT_STUFFING};
static const int COD_EDC[]  = {EDC_NENHUM, EDC_PARIDADE, EDC_CHECKSUM, EDC_CRC32, EDC_HAMMING};
static const int COD_MOD[]  = {0b001, 0b010, 0b100};
static const int COD_PORT[] = {0, 0b0001, 0b0010, 0b0100, 0b1000};

static const char* NOME_ENQ[]  = {"Contagem de caracteres", "Entrelaçamento de bits",
                                  "Flags + inserção de bytes", "Flags + inserção de bits"};
static const char* NOME_EDC[]  = {"Nenhum", "Bit de paridade", "Checksum", "CRC-32", "Hamming"};
static const char* NOME_MOD[]  = {"NRZ-polar", "Manchester", "Bipolar"};
static const char* NOME_PORT[] = {"Nenhuma (só banda base)", "ASK", "FSK", "8-PSK", "32-QAM"};

static std::string num(double v, int casas = 2) {
    char b[32];
    std::snprintf(b, sizeof b, "%.*f", casas, v);
    return b;
}

// caixa de texto somente leitura, com fonte monoespacada e rolagem
class CaixaTexto : public Gtk::ScrolledWindow {
public:
    explicit CaixaTexto(int altura) {
        set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
        set_size_request(-1, altura);
        tv_.set_editable(false);
        tv_.set_monospace(true);
        tv_.set_wrap_mode(Gtk::WRAP_CHAR);
        add(tv_);
    }
    void set_texto(const std::string& t) { tv_.get_buffer()->set_text(t); }

private:
    Gtk::TextView tv_;
};

// moldura com grafico rolavel na horizontal + controle de zoom
class PainelGrafico : public Gtk::Frame {
public:
    PainelGrafico(const std::string& titulo, double r, double g, double b)
        : Gtk::Frame(titulo), caixa_(Gtk::ORIENTATION_VERTICAL, 4),
          barra_(Gtk::ORIENTATION_HORIZONTAL, 6), zoom_(Gtk::ORIENTATION_HORIZONTAL), graf_(r, g, b) {
        set_margin_bottom(6);
        rolagem_.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_NEVER);
        rolagem_.set_size_request(-1, 245);
        rolagem_.add(graf_);

        rotulo_zoom_.set_text("Zoom horizontal:");
        zoom_.set_range(0.05, 3.0);
        zoom_.set_value(0.4);
        zoom_.set_digits(2);
        zoom_.signal_value_changed().connect([this] { graf_.set_zoom(zoom_.get_value()); });
        barra_.pack_start(rotulo_zoom_, Gtk::PACK_SHRINK);
        barra_.pack_start(zoom_, Gtk::PACK_EXPAND_WIDGET);

        caixa_.pack_start(rolagem_, Gtk::PACK_EXPAND_WIDGET);
        caixa_.pack_start(barra_, Gtk::PACK_SHRINK);
        add(caixa_);
    }
    void mostra(const std::vector<double>& dados, int amostras_por_bit,
                const std::vector<int>& bits = {}) {
        graf_.set_dados(dados, amostras_por_bit, bits);
    }

private:
    Gtk::Box            caixa_, barra_;
    Gtk::ScrolledWindow rolagem_;
    Gtk::Label          rotulo_zoom_;
    Gtk::Scale          zoom_;
    Grafico             graf_;
};

// ------------------------------------------------------- pagina de configuracao

class PaginaConfig : public Gtk::Grid {
public:
    PaginaConfig() {
        set_row_spacing(10);
        set_column_spacing(16);
        set_margin_start(24);
        set_margin_end(24);
        set_margin_top(20);
        set_margin_bottom(20);

        for (auto n : NOME_ENQ)  c_enq_.append(n);
        for (auto n : NOME_EDC)  c_edc_.append(n);
        for (auto n : NOME_MOD)  c_mod_.append(n);
        for (auto n : NOME_PORT) c_port_.append(n);

        s_tam_quadro_.set_range(1, 200);   // contagem usa 1 byte de cabecalho: maximo 255 bytes
        s_tam_quadro_.set_increments(1, 8);
        s_tam_edc_.set_range(8, 32);
        s_tam_edc_.set_increments(8, 8);
        s_apb_.set_range(2, 1000);
        s_apb_.set_increments(2, 10);
        s_tensao_.set_range(0.1, 10);
        s_tensao_.set_increments(0.1, 1);
        s_tensao_.set_digits(2);
        s_freq_.set_range(0.5, 20);
        s_freq_.set_increments(0.5, 1);
        s_freq_.set_digits(1);
        s_ruido_.set_range(0, 5);
        s_ruido_.set_increments(0.05, 0.5);
        s_ruido_.set_digits(2);

        int l = 0;
        secao("Camada de enlace", l);
        linha("Enquadramento", c_enq_, l);
        linha("Detecção/correção de erros (EDC)", c_edc_, l);
        linha("Tamanho máximo do quadro (bytes)", s_tam_quadro_, l);
        linha("Tamanho do checksum (bits)", s_tam_edc_, l);
        secao("Camada física", l);
        linha("Modulação em banda base", c_mod_, l);
        linha("Modulação de portadora", c_port_, l);
        linha("Amostras por bit", s_apb_, l);
        linha("Tensão (V)", s_tensao_, l);
        linha("Frequência da portadora (ciclos por bit)", s_freq_, l);
        secao("Canal", l);
        linha("Ruído gaussiano, desvio padrão (V)", s_ruido_, l);

        btn_padrao_.set_label("Restaurar padrões");
        btn_padrao_.set_halign(Gtk::ALIGN_START);
        btn_padrao_.signal_clicked().connect([this] { padrao(); });
        attach(btn_padrao_, 0, l++, 2, 1);

        dica_.set_markup("<i>As configurações valem para a próxima transmissão (aba TX).</i>");
        dica_.set_halign(Gtk::ALIGN_START);
        attach(dica_, 0, l++, 2, 1);

        padrao();
    }

    Config ler() const {
        Config c;
        c.enquadramento    = COD_ENQ[c_enq_.get_active_row_number()];
        c.edc              = COD_EDC[c_edc_.get_active_row_number()];
        c.tam_max_quadro   = s_tam_quadro_.get_value_as_int();
        c.tam_edc          = s_tam_edc_.get_value_as_int();
        c.modulacao        = COD_MOD[c_mod_.get_active_row_number()];
        c.portadora        = COD_PORT[c_port_.get_active_row_number()];
        c.amostras_por_bit = s_apb_.get_value_as_int();
        c.tensao           = s_tensao_.get_value();
        c.freq             = s_freq_.get_value();
        c.ruido            = s_ruido_.get_value();
        return c;
    }

    // texto curto para mostrar nas outras abas
    std::string resumo() const {
        Config c = ler();
        std::ostringstream os;
        os << "Enquadramento: " << NOME_ENQ[c_enq_.get_active_row_number()]
           << "  |  EDC: " << NOME_EDC[c_edc_.get_active_row_number()]
           << "  |  Banda base: " << NOME_MOD[c_mod_.get_active_row_number()]
           << "  |  Portadora: " << NOME_PORT[c_port_.get_active_row_number()]
           << "  |  " << c.amostras_por_bit << " amostras/bit"
           << "  |  " << num(c.tensao) << " V"
           << "  |  ruído σ = " << num(c.ruido) << " V";
        return os.str();
    }

private:
    void padrao() {
        c_enq_.set_active(0);
        c_edc_.set_active(0);
        c_mod_.set_active(0);
        c_port_.set_active(0);
        s_tam_quadro_.set_value(16);
        s_tam_edc_.set_value(8);
        s_apb_.set_value(100);
        s_tensao_.set_value(1.0);
        s_freq_.set_value(2.0);
        s_ruido_.set_value(0.0);
    }
    void secao(const std::string& titulo, int& l) {
        auto* t = Gtk::manage(new Gtk::Label());
        t->set_markup("<b>" + titulo + "</b>");
        t->set_halign(Gtk::ALIGN_START);
        t->set_margin_top(10);
        attach(*t, 0, l++, 2, 1);
    }
    void linha(const std::string& nome, Gtk::Widget& w, int& l) {
        auto* t = Gtk::manage(new Gtk::Label(nome));
        t->set_halign(Gtk::ALIGN_START);
        w.set_halign(Gtk::ALIGN_START);
        attach(*t, 0, l, 1, 1);
        attach(w, 1, l, 1, 1);
        l++;
    }

    Gtk::ComboBoxText c_enq_, c_edc_, c_mod_, c_port_;
    Gtk::SpinButton   s_tam_quadro_, s_tam_edc_, s_apb_, s_tensao_, s_freq_, s_ruido_;
    Gtk::Button       btn_padrao_;
    Gtk::Label        dica_;
};

// ------------------------------------------------------------ pagina do transmissor

class PaginaTx : public Gtk::Box {
public:
    std::function<void()> ao_transmitir;

    PaginaTx()
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8), linha_msg_(Gtk::ORIENTATION_HORIZONTAL, 8),
          f_payload_("Payload (bits da aplicação)"), payload_(70),
          f_quadros_("Quadros produzidos pela camada de enlace"), quadros_(120),
          bb_("Sinal em banda base", 0.10, 0.30, 0.80),
          port_("Sinal da portadora", 0.85, 0.40, 0.05) {
        set_margin_start(16);
        set_margin_end(16);
        set_margin_top(12);
        set_margin_bottom(12);

        rot_msg_.set_text("Mensagem:");
        ent_.set_text("Olá, mundo!");
        btn_.set_label("Transmitir");
        btn_.signal_clicked().connect([this] { if (ao_transmitir) ao_transmitir(); });
        ent_.signal_activate().connect([this] { if (ao_transmitir) ao_transmitir(); });
        linha_msg_.pack_start(rot_msg_, Gtk::PACK_SHRINK);
        linha_msg_.pack_start(ent_, Gtk::PACK_EXPAND_WIDGET);
        linha_msg_.pack_start(btn_, Gtk::PACK_SHRINK);

        resumo_.set_halign(Gtk::ALIGN_START);
        resumo_.set_line_wrap(true);
        avisos_.set_halign(Gtk::ALIGN_START);
        avisos_.set_line_wrap(true);

        f_payload_.add(payload_);
        f_quadros_.add(quadros_);

        pack_start(linha_msg_, Gtk::PACK_SHRINK);
        pack_start(resumo_, Gtk::PACK_SHRINK);
        pack_start(avisos_, Gtk::PACK_SHRINK);
        pack_start(f_payload_, Gtk::PACK_SHRINK);
        pack_start(f_quadros_, Gtk::PACK_SHRINK);
        pack_start(bb_, Gtk::PACK_SHRINK);
        pack_start(port_, Gtk::PACK_SHRINK);
    }

    std::string texto() const { return ent_.get_text().raw(); }

    void mostra(const ResultadoTx& tx, const std::string& resumo_cfg) {
        resumo_.set_text(resumo_cfg);

        payload_.set_texto(Simulador::formata_bits(tx.payload) + "\n(" +
                           std::to_string(tx.payload.size()) + " bits)");

        std::ostringstream os;
        for (size_t i = 0; i < tx.quadros.size(); i++)
            os << "Quadro " << i + 1 << " (" << tx.quadros[i].size() << " bits):\n"
               << Simulador::formata_bits(tx.quadros[i]) << "\n\n";
        os << "Total: " << tx.bits.size() << " bits transmitidos";
        quadros_.set_texto(os.str());

        std::string av;
        for (const auto& a : tx.avisos) av += "⚠ " + a + "\n";
        avisos_.set_text(av);

        bb_.mostra(tx.sinal_bb, tx.amostras_por_bit, tx.bits);
        port_.mostra(tx.sinal_port, tx.amostras_por_bit, tx.bits);
    }

private:
    Gtk::Box      linha_msg_;
    Gtk::Label    rot_msg_, resumo_, avisos_;
    Gtk::Entry    ent_;
    Gtk::Button   btn_;
    Gtk::Frame    f_payload_;
    CaixaTexto    payload_;
    Gtk::Frame    f_quadros_;
    CaixaTexto    quadros_;
    PainelGrafico bb_, port_;
};

// ------------------------------------------------------------ pagina do receptor

class PaginaRx : public Gtk::Box {
public:
    PaginaRx()
        : Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8),
          sinal_("Sinal recebido (após o canal, com ruído)", 0.10, 0.55, 0.25),
          f_bits_("Bits recuperados pela camada física"), bits_(70),
          f_quadros_("Quadros recuperados pela camada de enlace"), quadros_(110),
          f_msg_("Mensagem entregue à aplicação"), msg_(60) {
        set_margin_start(16);
        set_margin_end(16);
        set_margin_top(12);
        set_margin_bottom(12);

        status_.set_halign(Gtk::ALIGN_START);
        status_.set_line_wrap(true);
        status_.set_text("Nenhuma transmissão ainda. Use a aba Transmissor (TX).");

        f_bits_.add(bits_);
        f_quadros_.add(quadros_);
        f_msg_.add(msg_);

        pack_start(status_, Gtk::PACK_SHRINK);
        pack_start(sinal_, Gtk::PACK_SHRINK);
        pack_start(f_bits_, Gtk::PACK_SHRINK);
        pack_start(f_quadros_, Gtk::PACK_SHRINK);
        pack_start(f_msg_, Gtk::PACK_SHRINK);
    }

    void mostra(const ResultadoRx& rx, const std::vector<double>& sinal, int amostras_por_bit,
                const std::string& resumo_cfg) {
        sinal_.mostra(sinal, amostras_por_bit);

        std::string st = resumo_cfg + "\n";
        for (const auto& a : rx.avisos) st += "⚠ " + a + "\n";
        status_.set_text(st);

        if (!rx.implementado) {
            bits_.set_texto("(receptor não implementado)");
            quadros_.set_texto("(receptor não implementado)");
            msg_.set_texto("(receptor não implementado)");
            return;
        }
        bits_.set_texto(Simulador::formata_bits(rx.bits));
        std::ostringstream os;
        for (size_t i = 0; i < rx.quadros.size(); i++)
            os << "Quadro " << i + 1 << " (" << rx.quadros[i].size() << " bits):\n"
               << Simulador::formata_bits(rx.quadros[i]) << "\n\n";
        quadros_.set_texto(os.str());
        msg_.set_texto(rx.mensagem);
    }

private:
    Gtk::Label    status_;
    PainelGrafico sinal_;
    Gtk::Frame    f_bits_;
    CaixaTexto    bits_;
    Gtk::Frame    f_quadros_;
    CaixaTexto    quadros_;
    Gtk::Frame    f_msg_;
    CaixaTexto    msg_;
};

// ---------------------------------------------------------------- janela principal

class Janela : public Gtk::Window {
public:
    Janela() : raiz_(Gtk::ORIENTATION_VERTICAL, 0) {
        set_title("Simulador de Camadas Física e Enlace");
        set_default_size(1100, 820);

        esc_cfg_.add(cfg_);
        esc_tx_.add(tx_);
        esc_rx_.add(rx_);
        for (auto* e : {&esc_cfg_, &esc_tx_, &esc_rx_})
            e->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

        abas_.append_page(esc_cfg_, "Controle / Configuração");
        abas_.append_page(esc_tx_, "Transmissor (TX)");
        abas_.append_page(esc_rx_, "Receptor (RX)");

        status_.set_halign(Gtk::ALIGN_START);
        status_.set_margin_start(8);
        status_.set_margin_top(4);
        status_.set_margin_bottom(4);
        status_.set_text("Pronto.");

        raiz_.pack_start(abas_, Gtk::PACK_EXPAND_WIDGET);
        raiz_.pack_start(status_, Gtk::PACK_SHRINK);
        add(raiz_);

        tx_.ao_transmitir = [this] { transmitir(); };
        show_all_children();
    }

private:
    void transmitir() {
        const Config cfg = cfg_.ler();
        const std::string texto = tx_.texto();
        if (texto.empty()) {
            status_.set_text("Digite uma mensagem para transmitir.");
            return;
        }

        const ResultadoTx tx = sim_.transmitir(texto, cfg);
        const std::string resumo = cfg_.resumo();
        tx_.mostra(tx, resumo);

        const std::vector<double> recebido = sim_.canal(tx.sinal_transmitido(), cfg.ruido);
        const ResultadoRx rx = sim_.receber(tx, recebido, cfg);
        rx_.mostra(rx, recebido, tx.amostras_por_bit, resumo);

        status_.set_text("Transmitido: " + std::to_string(tx.payload.size()) + " bits de payload, " +
                         std::to_string(tx.quadros.size()) + " quadro(s), " +
                         std::to_string(tx.bits.size()) + " bits no meio.");
    }

    Simulador sim_;

    Gtk::Box            raiz_;
    Gtk::Notebook       abas_;
    Gtk::Label          status_;
    Gtk::ScrolledWindow esc_cfg_, esc_tx_, esc_rx_;
    PaginaConfig        cfg_;
    PaginaTx            tx_;
    PaginaRx            rx_;
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "br.unb.redes.simulador");
    Janela janela;
    return app->run(janela);
}
