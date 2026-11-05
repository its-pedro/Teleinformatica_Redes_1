#include "camadaFisica.hpp"

void camada_fisica_transmissor::modula_sinal(){
    set_constelacao();
    switch(get_portadora()){
    case 0b01:
        switch (get_modulacao()){
            case 0b001:
                nrz_polar();
                break;
            
            case 0b010:
                manchester();
                break;

            case 0b100:
                bipolar();
                break;

            default:
                break;
        }
    case 0b10:
        switch(get_modulacao()){
            case 0b0001:
                ask();
                break;
            
            case 0b0010:
                fsk();
                break;
            
            case 0b0100:
                psk8();
                break;

            case 0b1000:
                qam32();
                break;

            default:
                break;
            
        }
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
    int freq = get_cut_freq();
    double v = get_input_tension();
    
    for(int bit : bits){
        for(int i = 0; i < smp; i++){
            double val = (bit == 1) ? 
                v * sin((2*pi*freq*i)/((double) smp)) : 0;
            signal.push_back(val);
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
    
    for(int bit : bits){
        for(int i = 0; i < smp; i++){
            double val = (bit == 1) ? 
                v * sin((4*pi*freq*i)/((double) smp)): 
                v * sin((2*pi*freq*i)/((double) smp));
            signal.push_back(val);
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

        simbolo_iq np_signal = constelacao[valor_binario];

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

        simbolo_iq np_signal = constelacao[valor_binario];

        for(int n = 0; n < get_amostragem(); n++){
            signal.push_back(np_signal.fase);
            signal.push_back(np_signal.quadratura);
        }
    }
    set_modulated_signal(signal);
}
