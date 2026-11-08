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
                zero++;
            }else{
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
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int smp = get_amostragem();
    int size = signal.size();
    vector<double> medias_energeticas;

    for(int i = 0; i < size; i+= smp){
        double energia_total = 0.0;

        for(int j = 0; j < smp; j++)
            energia_total += abs(signal[i+j]);
        
        medias_energeticas.push_back(energia_total / smp);
    }
    double minApl = *min_element(medias_energeticas.begin(), medias_energeticas.end());
    double maxApl = *max_element(medias_energeticas.begin(), medias_energeticas.end());

    double dinamicLim = (maxApl - minApl) / 2.0;

    for(auto avg : medias_energeticas){
        if(avg >= dinamicLim)
            bits.push_back(1);
        else
            bits.push_back(0);
    }
    set_bit_beacon(bits);
}

void camada_fisica_receptor::fsk(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int smp = get_amostragem();
    double fc = get_cut_freq();
    int size = signal.size();

    for(int i = 0; i < size; i+= smp){
        double energiaF0 = 0;
        double energiaF1 = 0;

        for(int j = 0; j < smp; j++){
            double tempo = (((double) j) / (double) smp);
            double refFc  = sin(2.0 * pi * fc * tempo);
            double ref2Fc = sin(4.0 * pi * fc * tempo);

            energiaF0  += abs(signal[i+j] * refFc);
            energiaF1 += abs(signal[i+j] * ref2Fc);
        }

        if(energiaF1 >= energiaF0)
            bits.push_back(1);
        else
            bits.push_back(0);
    }
    set_bit_beacon(bits);
}

void camada_fisica_receptor::psk8(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int size = signal.size();
    int smp = 2 * get_amostragem();

    for(int i = 0; i < size; i += smp){
        double fase_media = 0, quadratura_media = 0;

        for(int j = 0; j < smp-1; j += 2){
            fase_media += signal[i+j];
            quadratura_media += signal[i+j+1];
        }
        fase_media /= ((double)smp / 2.0);
        quadratura_media /= ((double)smp / 2.0);

        double menor_dist = 1e9;
        int melhor_idx_gray = 0;

        for(int i = 0; i < 8; i++){
            double di = fase_media - constelacao[i].fase;
            double dq = quadratura_media - constelacao[i].quadratura;
            double dist_qi = (dq*dq) + (di*di);

            if(dist_qi < menor_dist){
                menor_dist = dist_qi;
                melhor_idx_gray = i;
            }
        }

        int simbolo = inv_gray[melhor_idx_gray];
        bits.push_back((simbolo >> 2) & 1);
        bits.push_back((simbolo >> 1) & 1);
        bits.push_back(simbolo & 1);
    }
    set_bit_beacon(bits);
}

void camada_fisica_receptor::qam32(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int size = signal.size();
    int smp = 2 * get_amostragem();

    for(int i = 0; i < size; i += smp){
        double fase_media = 0, quadratura_media = 0;

        for(int j = 0; j < smp-1; j += 2){
            fase_media += signal[i+j];
            quadratura_media += signal[i+j+1];
        }
        fase_media /= ((double)smp / 2.0);
        quadratura_media /= ((double)smp / 2.0);

        double menor_dist = 1e9;
        int melhor_idx_gray = 0;

        for(int i = 0; i < 32; i++){
            double di = fase_media - constelacao[i].fase;
            double dq = quadratura_media - constelacao[i].quadratura;
            double dist_qi = (dq*dq) + (di*di);

            if(dist_qi < menor_dist){
                menor_dist = dist_qi;
                melhor_idx_gray = i;
            }
        }

        int simbolo = inv_gray[melhor_idx_gray];
        bits.push_back((simbolo >> 4) & 1);
        bits.push_back((simbolo >> 3) & 1);
        bits.push_back((simbolo >> 2) & 1);
        bits.push_back((simbolo >> 1) & 1);
        bits.push_back(simbolo & 1);
    }
    set_bit_beacon(bits);
}