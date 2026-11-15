#include "camadaFisica.hpp"

// Construtor parametrizado que atribui os valores do 
// parametros durante a inicializacao da classe

camada_fisica_receptor::camada_fisica_receptor
(int portadora_banda_base, int cod_modulacao, int amostras_por_simbolo, double frequencia_corte, double input_tension){
    set_portadora(portadora_banda_base);
    set_modulacao(cod_modulacao);
    set_amostragem(amostras_por_simbolo);
    set_cut_freq(frequencia_corte);
    set_input_tension(input_tension);
}

// Rotina principal do receptor que a partir dos codigos seleciona
// o tipo de modulacao utilizada banda base ou portadora
// e dependendo do tipo utilizado seleciona o algoritmo de modulacao desejado

void camada_fisica_receptor::demodula_sinal(){
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

// Decodifica o padrao de modulacao banda-base Not-Return to Zero 
// no qual o sinal = +V (1) ou -V (0)
// e V eh a tensao de input do transmissor/receptor

void camada_fisica_receptor::nrz_polar(){
    vector<int> beacon;
    vector<double> signal = get_modulated_signal();
    int smp = get_amostragem();
    int size = signal.size();

    // O sinal eh percorrido por todas as smp amostragens de um bit e
    // quando o valor for maior que a media do sinal (0)
    // ele eh computado como 1, caso contrario como 0.

    // Apos percorrer as smp amostras o valor mais provavel do bit 
    // eh o bit que tiver sido mais vezes computado 

    for(int i = 0; i < size; i += smp){
        int one = 0, zero = 0;

        for(int j = 0; j < smp; j++)
           signal[i+j] >= 0 ? one++ : zero++;

        int bit = (one >= zero) ? 1 : 0;
        beacon.push_back(bit);
    }
    set_bit_beacon(beacon);
}

// Decodifica o padrao de modulacao banda-base Manchester
// Para cada bit enviado eh realizado a operacao xor entre o bit
// a ser enviado e o sinal de clock do sitema 

void camada_fisica_receptor::manchester(){
    vector<int> beacon;
    vector<double> signal = get_modulated_signal();
    int frq = 2 * get_amostragem();
    double v = get_input_tension();
    int size = signal.size();

    for(int i = 0; i < size; i += frq){
        int one = 0, zero = 0;

        // O sinal eh percorrido por todas as smp amostras de um bit e
        // Para a primeira metade do sinal como eh realizada  
        // xor com bit 0 o valor do bit sera "equivalente" ao valor do sinal

        // Se o sinal >= freq/2 entao o bit mais provavel eh 1 
        // caso contrario bit mais provavel eh 0
        for(int j = 0; j < frq / 2; j++)
            (signal[i+j] / v) >= 0.5 ? one++ : zero++;

        // Ja na segunda metade do sinal como eh realizada xor com  bit 1
        // Se o sinal >= freq/2 entao o bit mais provavel eh 0 
        // caso contrario bit mais provavel eh 1
        for(int j = frq /2; j < frq; j++)
            (signal[i+j] / v) >= 0.5 ? zero++ : one++;

        // o bit predito a maior quantidade de vezes eh o valor provavel do bit
        int bit = (one >= zero) ? 1 : 0;
        beacon.push_back(bit);
    }
    set_bit_beacon(beacon);
}

// Decodifica o padrao de modulacao banda-base Bipolar

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
                if(signal[i+j] > 0 && get_last_bipolar() < 0)
                    one++;
                else if(signal[i+j] < 0 && get_last_bipolar() > 0)
                    one++;
                else
                    ;//cout<< i+j << " error\n";
            }
        }

        int bit = (one >= zero) ? 1 : 0;
        if(bit == 1) set_last_bipolar(- (get_last_bipolar()));
        beacon.push_back(bit);
    }
    set_bit_beacon(beacon);
}

// Decodifica o padrao de codificacao por portadora ASK

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

// Decodifica o padrao de codificacao por portadora FSK

void camada_fisica_receptor::fsk(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int smp = get_amostragem();
    double fc = get_cut_freq();
    int size = signal.size();

    double tempo = 0.0;
    double dt = 1.0 / (8 * fc);

    for(int i = 0; i < size; i+= smp){
        double energiaF0 = 0;
        double energiaF1 = 0;

        for(int j = 0; j < smp; j++){
            double refFc  = sin(2.0 * pi * fc * tempo);
            double ref2Fc = sin(4.0 * pi * fc * tempo);

            energiaF0  += abs(signal[i+j] * refFc);
            energiaF1 += abs(signal[i+j] * ref2Fc);
            tempo += dt;
        }

        if(energiaF1 >= energiaF0)
            bits.push_back(1);
        else
            bits.push_back(0);
    }
    set_bit_beacon(bits);
}

// Decodifica o padrao de codificacao por portadora 8PSK

void camada_fisica_receptor::psk8(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int size = signal.size();
    int smp = 2 * get_amostragem();

    double freq = get_cut_freq();
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);

    for(int i = 0; i < size; i += smp){
        double fase_media = 0, quadratura_media = 0;

        for(int j = 0; j < smp-1; j += 2){
            double canalI = cos(2 * pi * freq * tempo);
            double canalQ = -sin(2 * pi * freq * tempo);

            signal[i+j] /= (canalI != 0) ? canalI : 1;
            signal[i+j+1] /= (canalQ != 0)? canalQ : 1;

            fase_media += signal[i+j];
            quadratura_media += signal[i+j+1];

            tempo += dt;
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

// Decodifica o padrao de codificacao por portadora 32QAM

void camada_fisica_receptor::qam32(){
    vector<double> signal = get_modulated_signal();
    vector<int> bits;
    int size = signal.size();
    int smp = 2 * get_amostragem();

    double freq = get_cut_freq();
    double tempo = 0.0;
    double dt = 1.0 / (8 * freq);

    for(int i = 0; i < size; i += smp){
        double fase_media = 0, quadratura_media = 0;

        for(int j = 0; j < smp-1; j += 2){
            double canalI = cos(2 * pi * freq * tempo);
            double canalQ = -sin(2 * pi * freq * tempo);

            signal[i+j] /= (canalI != 0) ? canalI : 1;
            signal[i+j+1] /= (canalQ != 0)? canalQ : 1;

            fase_media += signal[i+j];
            quadratura_media += signal[i+j+1];

            tempo += dt;
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