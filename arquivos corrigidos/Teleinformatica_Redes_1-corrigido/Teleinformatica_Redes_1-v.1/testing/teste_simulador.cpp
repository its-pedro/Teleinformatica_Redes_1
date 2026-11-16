// Teste sem interface grafica: TX -> canal -> RX para todas as combinacoes.
#include <cstdio>
#include "simulador.hpp"

int main() {
    const int ENQ[]  = {ENQ_CONTAGEM, ENQ_ENTRELACAMENTO, ENQ_BYTE_STUFFING, ENQ_BIT_STUFFING};
    const int EDC[]  = {EDC_NENHUM, EDC_PARIDADE, EDC_CHECKSUM, EDC_CRC32, EDC_HAMMING};
    const int MOD[]  = {NRZ_POLAR, MANCHESTER, BIPOLAR};
    const int PORT[] = {0, ASK, FSK, PSK8, QAM32};
    const std::string msg = "Ola, mundo! Teste do simulador.";

    Simulador sim;
    int total = 0, falhas = 0;
    for (int p : PORT)
        for (int m : MOD) {
            if (p != 0 && m != MOD[0]) continue;   // banda base irrelevante com portadora
            for (int e : ENQ)
                for (int d : EDC) {
                    Config c;
                    c.enquadramento = e; c.edc = d; c.modulacao = m; c.portadora = p;
                    ResultadoTx tx = sim.transmitir(msg, c);
                    auto rx = sim.receber(tx, sim.canal(tx.sinal_transmitido(), 0.0), c);
                    total++;
                    if (rx.mensagem != msg) {
                        falhas++;
                        std::printf("FALHA enq=%d edc=%d mod=%d port=%d\n", e, d, m, p);
                    }
                }
        }
    std::printf("%d combinacoes, %d falha(s)\n", total, falhas);
    return falhas ? 1 : 0;
}
