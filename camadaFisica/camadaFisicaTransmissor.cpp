#include "camadaFisica.hpp"

// Construtor parametrizado que atribui os valores do 
// parametros durante a inicializacao da classe

camada_fisica_transmissor::camada_fisica_transmissor
(int portadora_banda_base, int cod_modulacao, int amostras_por_simbolo, double frequencia_corte, double input_tension){
    set_portadora(portadora_banda_base);
    set_modulacao(cod_modulacao);
    set_amostragem(amostras_por_simbolo);
    set_cut_freq(frequencia_corte);
    set_input_tension(input_tension);
}    

// Rotina principal do transmissor que a partir dos codigos seleciona
// o tipo de modulacao utilizada banda base ou portadora
// e dependendo do tipo utilizado seleciona o algoritmo de modulacao desejado

void camada_fisica_transmissor::modula_sinal(){
    // Chama subrotina que constroi as constelacoe de simbolos no caso
    // dos algoritmos de modulacoes por portadora: 32 qam e 8 psk 
    set_constelacao();

    switch(get_portadora()){
    case BANDA_BASE:
        switch (get_modulacao()){
            case NRZ_POLAR: nrz_polar(); break;
            case MANCHESTER: manchester(); break;
            case BIPOLAR: bipolar(); break;
            default: break;
        }
        break;

    case PORTADORA:
        switch(get_modulacao()){
            case ASK: ask(); break;
            case FSK: fsk(); break;
            case PSK8: psk8(); break;
            case QAM32: qam32(); break;
            default: break;   
        }
        break;

    default: break;
    }
}

// Codifica o padrao de modulacao banda-base Not-Return to Zero 
// no qual o bit 1 eh representado por +V e o 0 por -V
// e V eh a tensao de input do transmissor/receptor

void camada_fisica_transmissor::nrz_polar(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    double v = get_input_tension();

    for(int bit : bits){
        double tension = (bit == 1)? v : -(v);

        for(int i = 0; i < get_amostragem(); i++){
            signal.push_back(tension);
        }
    }
    set_modulated_signal(signal);
}

// Codifica o padrao de modulacao banda-base Manchester
// Para cada bit enviado eh realizado a operacao xor entre o bit
// a ser enviado e o sinal de clock do sitema 

void camada_fisica_transmissor::manchester(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    double v = get_input_tension();
    int freq = 2 * get_amostragem();

    // Para simular o sinal de clock assume-se calcula-se a taxa de amostragem
    // por bit como o dobro da taxa de amostragem por simbolo
    // De zero a freq/2 eh calculado bit ^ 0 e o resultado eh multiplicado por V
    // De freq/2 a freq eh calculado bit ^ 1 e o resultado eh multiplicado por V

    for(int bit : bits){
        for(int i = 0; i < freq / 2; i++){
            int val = bit ^ 0;
            signal.push_back(val * v);
        }

        for(int i = freq /2; i < freq ; i++){
            int val = bit ^ 1;
            signal.push_back(val * v);
        }
    }
    set_modulated_signal(signal);
}

// Codifica o padrao de modulacao banda-base Bipolar
// onde o sinal = 0 V quando o bit for 0 e quando o bit for 1
// o sinal recebe alternadamente V e -V;

void camada_fisica_transmissor::bipolar(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    double v = get_input_tension();
    double tension;

    for(int bit : bits){
        tension = 0;
        
        // Utiliza-se a variavel last_bipolar para guardar o ultimo
        // estado (positivo ou negativo) utilizado pelo bit 1
        if(bit == 1) {
            tension = -(get_last_bipolar()*v);
            set_last_bipolar(-(get_last_bipolar()));
        }

        for(int i = 0; i < get_amostragem(); i++){
            signal.push_back(tension);
        }
    }
    set_modulated_signal(signal);
}

// Codifica o padrao de modulacao por portadora ASK
// a portadora eh modelada a partir de uma senoide cujo w = 2*pi*freq
// assim quando bit = 0 o sinal = 0 e quando bit = 1 o sinal = valor da senoide

void camada_fisica_transmissor::ask(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    int smp = get_amostragem();
    double freq = get_cut_freq();
    double v = get_input_tension();
    
    // para dar resolucao a onda a senoide eh amostrada 8 vezes ao longo do periodo
    // para que a informacao carregada por ela possa ser recuperada 
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);
    
    for(auto bit : bits){
        for(int i = 0; i < smp; i++){
            double val = (bit == 1) ? 
                v * sin(2 * pi * freq * tempo) : 0;
            signal.push_back(val);
            tempo += dt;
        }
    }
    set_modulated_signal(signal);
}

// Codifica o padrao de modulacao por portadora FSK
// a portadora eh modelada a partir de duas senoide 
// uma com w1 = 2*pi*freq e outra com w2 = 2*w1 = 2*pi*freq
// assim quando bit = 0 o sinal = valor da senoide vel angular w1
// e quando bit = 1 o sinal = valor da senoide de vel angular w2

void camada_fisica_transmissor::fsk(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    int smp = get_amostragem();
    double freq = get_cut_freq();
    double v = get_input_tension();

    // para dar resolucao a onda a senoide eh amostrada 8 vezes ao longo do periodo
    // para que a informacao carregada por ela possa ser recuperada 
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);
    
    for(int bit : bits){
        for(int i = 0; i < smp; i++){
            double val = (bit == 1) ? 
                v * sin(4 * pi * freq * tempo): 
                v * sin(2 * pi * freq * tempo);
            signal.push_back(val);
            tempo += dt;
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::psk8(){
    vector<double> signal;
    vector<int> bit = get_bit_beacon();
    int size = bit.size();    

    double freq = get_cut_freq();
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);

    for(int i = 0; i < size; i += 3){
        int valor_binario = 0;

        for(int j = 0; j < 3; j++){
            valor_binario <<= 1;

            if(i + j < size)
                valor_binario |= bit[i+j];
            else
                valor_binario |= 0;
        }
        simbolo_iq np_signal = constelacao[gray[valor_binario]];

        for(int n = 0; n < get_amostragem(); n++){
            double canalI = cos(2 * pi * freq * tempo);
            double canalQ = -sin(2 * pi * freq * tempo);

            signal.push_back(np_signal.fase * canalI);
            signal.push_back(np_signal.quadratura * canalQ);

            tempo += dt;
        }
        
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::qam32(){
    vector<double> signal;
    vector<int> bit = get_bit_beacon();
    int size = bit.size(); 
    
    double freq = get_cut_freq();
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);

    for(int i = 0; i < size; i += 5){
        int valor_binario = 0;

        for(int j = 0; j < 5; j++){
            valor_binario <<= 1;

            if(i + j < size)
                valor_binario |= bit[i+j];
            else
                valor_binario |= 0;
        }
        simbolo_iq np_signal = constelacao[gray[valor_binario]];

        for(int n = 0; n < get_amostragem(); n++){
            double canalI = cos(2 * pi * freq * tempo);
            double canalQ = -sin(2 * pi * freq * tempo);

            signal.push_back(np_signal.fase * canalI);
            signal.push_back(np_signal.quadratura * canalQ);

            tempo += dt;
        }
    }
    set_modulated_signal(signal);
}
