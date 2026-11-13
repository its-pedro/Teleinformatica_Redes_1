#ifndef GRAFICO_HPP
#define GRAFICO_HPP

// Widget que desenha um sinal (vector<double>) com cairo.
// - eixo central e grade vertical a cada bit
// - rotulos 0/1 acima de cada bit (quando ha espaco)
// - com muitas amostras por pixel, desenha o envelope (min/max por coluna)

#include <gtkmm.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

class Grafico : public Gtk::DrawingArea {
public:
    Grafico(double r, double g, double b) : r_(r), g_(g), b_(b) { set_size_request(400, 220); }

    void set_dados(const std::vector<double>& dados, int amostras_por_bit,
                   const std::vector<int>& bits = {}) {
        dados_ = dados;
        apb_ = amostras_por_bit;
        bits_ = bits;
        atualiza();
    }
    void set_zoom(double pixels_por_amostra) { zoom_ = pixels_por_amostra; atualiza(); }

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
        const int w = get_allocated_width();
        const int h = get_allocated_height();
        const size_t n = dados_.size();

        cr->set_source_rgb(1, 1, 1);
        cr->paint();

        if (n < 2) {
            cr->set_source_rgb(0.5, 0.5, 0.5);
            cr->set_font_size(12);
            cr->move_to(12, h / 2.0);
            cr->show_text("(sem dados)");
            return true;
        }

        const double mid = (h + 16) / 2.0;          // espaco no topo para os rotulos
        const double amp = (h - 16) / 2.0 - 8.0;     // amplitude maxima em pixels
        double ymax = 0;
        for (double v : dados_) ymax = std::max(ymax, std::fabs(v));
        if (ymax < 1e-9) ymax = 1;
        auto py = [&](double v) { return mid - v / ymax * amp; };
        const double px_por_amostra = (double)w / (double)n;

        // grade vertical a cada bit
        if (apb_ > 0) {
            const double px_bit = apb_ * px_por_amostra;
            if (px_bit >= 6) {
                cr->set_source_rgb(0.88, 0.88, 0.88);
                cr->set_line_width(1.0);
                for (size_t i = 0; i * apb_ <= n; i++) {
                    double x = i * apb_ * px_por_amostra;
                    cr->move_to(x, 16);
                    cr->line_to(x, h);
                }
                cr->stroke();
            }
            if (px_bit >= 14 && !bits_.empty()) {
                cr->set_source_rgb(0.3, 0.3, 0.3);
                cr->set_font_size(11);
                for (size_t i = 0; i < bits_.size(); i++) {
                    cr->move_to((i + 0.5) * px_bit - 3, 12);
                    cr->show_text(bits_[i] ? "1" : "0");
                }
            }
        }

        // eixo central
        cr->set_source_rgb(0.65, 0.65, 0.65);
        cr->set_line_width(1.0);
        cr->move_to(0, mid);
        cr->line_to(w, mid);
        cr->stroke();

        // sinal
        cr->set_source_rgb(r_, g_, b_);
        cr->set_line_width(1.5);
        if (n <= 2 * (size_t)w) {
            cr->move_to(0, py(dados_[0]));
            for (size_t i = 1; i < n; i++) cr->line_to(i * px_por_amostra, py(dados_[i]));
        } else {
            for (int x = 0; x < w; x++) {
                size_t a = (size_t)((double)x / w * n);
                size_t b = std::min(n, (size_t)((double)(x + 1) / w * n) + 1);
                double lo = dados_[a], hi = dados_[a];
                for (size_t i = a; i < b; i++) { lo = std::min(lo, dados_[i]); hi = std::max(hi, dados_[i]); }
                cr->move_to(x + 0.5, py(lo));
                cr->line_to(x + 0.5, py(hi));
            }
        }
        cr->stroke();

        // escala
        char txt[40];
        std::snprintf(txt, sizeof txt, "escala: +-%.2f V", ymax);
        cr->set_source_rgb(0.4, 0.4, 0.4);
        cr->set_font_size(10);
        cr->move_to(4, h - 4);
        cr->show_text(txt);
        return true;
    }

private:
    void atualiza() {
        int largura = (int)(dados_.size() * zoom_);
        largura = std::max(400, std::min(largura, 30000));
        set_size_request(largura, 220);
        queue_draw();
    }

    std::vector<double> dados_;
    std::vector<int>    bits_;
    int    apb_  = 0;
    double zoom_ = 0.4;
    double r_, g_, b_;
};

#endif
