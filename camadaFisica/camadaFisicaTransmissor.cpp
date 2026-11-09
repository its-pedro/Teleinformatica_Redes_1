#include "camadaFisica.hpp"

camada_fisica_transmissor::camada_fisica_transmissor
(int portadora_banda_base, int cod_modulacao, int amostras_por_simbolo, double frequencia_corte, double input_tension){
    set_portadora(portadora_banda_base);
    set_modulacao(cod_modulacao);
    set_amostragem(amostras_por_simbolo);
    set_cut_freq(frequencia_corte);
    set_input_tension(input_tension);
}    

void camada_fisica_transmissor::modula_sinal(){
    set_constelacao();
    switch(get_portadora()){
    case BANDA_BASE:
        switch (get_modulacao()){
            case NRZ_POLAR:
                nrz_polar();
                break;
            
            case MANCHESTER:
                manchester();
                break;

            case BIPOLAR:
                bipolar();
                break;

            default:
                break;
        }
        break;
    case PORTADORA:
        switch(get_modulacao()){
            case ASK:
                ask();
                break;
            
            case FSK:
                fsk();
                break;
            
            case PSK8:
                psk8();
                break;

            case QAM32:
                qam32();
                break;

            default:
                break;   
        }
        break;
    default:
        break;
    }
}

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

void camada_fisica_transmissor::manchester(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    double v = get_input_tension();
    int freq = 2 * get_amostragem();

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

void camada_fisica_transmissor::bipolar(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    double v = get_input_tension();
    double tension;

    for(int bit : bits){
        tension = 0;
        
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

void camada_fisica_transmissor::ask(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    int smp = get_amostragem();
    double freq = get_cut_freq();
    double v = get_input_tension();
    
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

void camada_fisica_transmissor::fsk(){
    vector<double> signal;
    vector<int> bits = get_bit_beacon();
    int smp = get_amostragem();
    int freq = get_cut_freq();
    double v = get_input_tension();

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
            signal.push_back(np_signal.fase);
            signal.push_back(np_signal.quadratura);
        }
        
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::qam32(){
    vector<double> signal;
    vector<int> bit = get_bit_beacon();
    int size = bit.size();   

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
            signal.push_back(np_signal.fase);
            signal.push_back(np_signal.quadratura);
        }
    }
    set_modulated_signal(signal);
}
