#include "camadaFisica.hpp"

void camada_fisica_receptor::demodula_sinal(){
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

void camada_fisica_receptor::nrz_polar(){
    vector<int> beacon;
    vector<double> signal = get_modulated_signal();
    int smp = get_amostragem();
    double v = get_input_tension();
    int size = signal.size();


    for(int i = 0; i < size; i += smp){
        int one = 0, zero = 0;

        for(int j = 0; j < smp; j++)
           signal[i+j] >= 0 ? one++ : zero++;

        int bit = (one >= zero) ? 1 : 0;
        beacon.push_back(bit);
    }
    set_bit_beacon(beacon);
}

void camada_fisica_receptor::manchester(){
    vector<int> beacon;
    vector<double> signal = get_modulated_signal();
    int frq = 2 * get_amostragem();
    double v = get_input_tension();
    int size = signal.size();

    for(int i = 0; i < size; i += frq){
        int one = 0, zero = 0;

        for(int j = 0; j < frq / 2; j++)
            (signal[i+j] / v) >= 0.5 ? one++ : zero++;

        for(int j = frq /2; j < frq; j++)
            (signal[i+j] / v) >= 0.5 ? zero++ : one++;

        int bit = (one >= zero) ? 1 : 0;
        beacon.push_back(bit);
    }
}

void camada_fisica_receptor::bipolar(){
    vector<int> beacon;
    vector<double> signal = get_modulated_signal();
    int smp = get_amostragem();
    double v = get_input_tension();
    int size = signal.size();

    for(int i = 0; i < size; i += smp){
        int one = 0, zero = 0;

        for(int j = 0; j < smp; j++){
            if(abs(signal[i+j]) < (v/2)){
                // bit 0
                zero++;
            }else{
                // bit 1
                if(signal[i+j] < 0 && get_last_bipolar() < 0)
                    one++;
                else if(signal[i+j] > 0 && get_last_bipolar() > 0)
                    one++;
                else
                    cout<< "error\n";
            }
        }

        int bit = (one >= zero) ? 1 : 0;
        if(bit == 1) set_last_bipolar(- (get_last_bipolar()));
        beacon.push_back(bit);
    }
    set_bit_beacon(beacon);
}

void camada_fisica_receptor::ask(){
   
}

void camada_fisica_receptor::fsk(){
    
}

void camada_fisica_receptor::psk8(){
    
}

void camada_fisica_receptor::qam32(){

}