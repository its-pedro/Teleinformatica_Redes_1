#include "camadaFisica.hpp"

void camada_fisica_transmissor::modula_sinal(){
    switch (this->cod_modulacao){
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
}

void camada_fisica_transmissor::modula_portadora(){
    switch(this->cod_portadora){
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
}

void camada_fisica_transmissor::nrz_polar(){
    this->modulated_signal.clear();

    for(int bit : bit_beacon){
        double tension = (bit == 1)? this->input_tension : -(this->input_tension);

        for(int i = 0; i < this->amostras_por_bit; i++){
            this->modulated_signal.push_back(tension);
        }
    }
}

void camada_fisica_transmissor::manchester(){
    this->modulated_signal.clear();

    for(int bit : bit_beacon){
        for(int i = 0; i < this->amostras_por_bit/2; i++){
            int val = bit ^ 0;
            this->modulated_signal.push_back(val*this->input_tension);
        }

        for(int i = this->amostras_por_bit/2; i < this->amostras_por_bit; i++){
            int val = bit ^ 1;
            this->modulated_signal.push_back(val*this->input_tension);
        }
    }
}

void camada_fisica_transmissor::bipolar(){
    this->modulated_signal.clear();
    double tension;
    double ones_last = -input_tension;

    for(int bit : bit_beacon){
        tension = 0;
        
        if(bit == 1) {
            tension = -ones_last;
            ones_last= tension;
        }

        for(int i = 0; i < this->amostras_por_bit; i++){
            this->modulated_signal.push_back(tension);
        }
    }
}

void camada_fisica_transmissor::ask(){
    this->modulated_port.clear();
    
    for(int bit : bit_beacon){
        for(int i = 0; i < this->amostras_por_bit; i++){
            double signal = (bit == 1) ? 
                this->input_tension * sin((2*pi*this->cut_freq*i)/((double) this->amostras_por_bit)) : 0;
            this->modulated_port.push_back(signal);
        }
    }
}

void camada_fisica_transmissor::fsk(){
    this->modulated_port.clear();
    
    for(int bit : bit_beacon){
        for(int i = 0; i < this->amostras_por_bit; i++){
            double signal = (bit == 1) ? 
                this->input_tension * sin((4*pi*this->cut_freq*i)/((double) this->amostras_por_bit)) : 
                this->input_tension * sin((2*pi*this->cut_freq*i)/((double) this->amostras_por_bit));
            this->modulated_port.push_back(signal);
        }
    }
}

void camada_fisica_transmissor::psk8(){
    this->modulated_port.clear();
    

}

void camada_fisica_transmissor::qam32(){
    this->modulated_port.clear();
}