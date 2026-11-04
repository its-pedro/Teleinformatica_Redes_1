#include "camadaFisica.hpp"

void camada_fisica_transmissor::modula_sinal(){
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

    for(int bit : get_bit_beacon()){
        double tension = (bit == 1)? get_input_tension() : -(get_input_tension());

        for(int i = 0; i < get_amostragem(); i++){
            signal.push_back(tension);
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::manchester(){
    vector<double> signal;

    for(int bit : get_bit_beacon()){
        for(int i = 0; i < get_amostragem()/2; i++){
            int val = bit ^ 0;
            signal.push_back(val*get_input_tension());
        }

        for(int i = get_amostragem()/2; i < get_amostragem(); i++){
            int val = bit ^ 1;
            signal.push_back(val*get_input_tension());
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::bipolar(){
    vector<double> signal;
    double tension;

    for(int bit : get_bit_beacon()){
        tension = 0;
        
        if(bit == 1) {
            tension = -(get_last_bipolar()*get_input_tension());
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
    
    for(int bit : get_bit_beacon()){
        for(int i = 0; i < get_amostragem(); i++){
            double val = (bit == 1) ? 
                get_input_tension() * sin((2*pi*get_cut_freq()*i)/((double) get_amostragem())) : 0;
            signal.push_back(val);
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::fsk(){
    vector<double> signal;
    
    for(int bit : get_bit_beacon()){
        for(int i = 0; i < get_amostragem(); i++){
            double val = (bit == 1) ? 
                get_input_tension() * sin((4*pi*get_cut_freq()*i)/((double) get_amostragem())) : 
                get_input_tension() * sin((2*pi*get_cut_freq()*i)/((double) get_amostragem()));
            signal.push_back(val);
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::psk8(){
    vector<double> signal;
    int size = get_bit_beacon().size();   

    for(int i = 0; i < size; i++){
        int indice = 0;

        for(int j = 2; j >= 0; j--){
            if(i + (2-j) >= size){
                indice |= 0 << j;
            }else{
                indice |= (get_bit_beacon()[i+(2-j)] << j); 
            }
        }
    }
    set_modulated_signal(signal);
}

void camada_fisica_transmissor::qam32(){
    vector<double> signal;


    set_modulated_signal(signal);
}