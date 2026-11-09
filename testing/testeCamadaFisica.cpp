#include "testeCamadaFisica.hpp"

void teste_camada_fisica::run(){
    cout << "=== INICIANDO TESTES DA CAMADA FÍSICA ===\n" << endl;

    testa_nrz_polar();
    testa_manchester();
    testa_bipolar();
    testa_ask();
    testa_fsk();
    testa_8psk();
    testa_32qam();

    cout << "=== FIM DOS TESTES DA CAMADA FÍSICA ===\n" << endl;
}

//validado
void teste_camada_fisica::testa_nrz_polar(){
    tx = new camada_fisica_transmissor(BANDA_BASE, NRZ_POLAR, 5, 60, 5);
    rx = new camada_fisica_receptor(BANDA_BASE, NRZ_POLAR, 5, 60, 5);

    cout << "=== Teste NRZ Polar ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//validado
void teste_camada_fisica::testa_manchester(){
    tx = new camada_fisica_transmissor(BANDA_BASE, MANCHESTER, 5, 60, 5);
    rx = new camada_fisica_receptor(BANDA_BASE, MANCHESTER, 5, 60, 5);

    cout << "=== Teste Manchester ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//validado
void teste_camada_fisica::testa_bipolar(){
    tx = new camada_fisica_transmissor(BANDA_BASE, BIPOLAR, 5, 60, 5);
    rx = new camada_fisica_receptor(BANDA_BASE, BIPOLAR, 5, 60, 5);

    cout << "=== Teste Bipolar ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//validado
void teste_camada_fisica::testa_ask(){
    tx = new camada_fisica_transmissor(PORTADORA, ASK, 10, 60.0, 5.0);
    rx = new camada_fisica_receptor(PORTADORA, ASK, 10, 60.0, 5.0);

    cout << "=== Teste Ask ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();

    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//validado
void teste_camada_fisica::testa_fsk(){
    tx = new camada_fisica_transmissor(PORTADORA, FSK, 10, 60, 5);
    rx = new camada_fisica_receptor(PORTADORA, FSK, 10, 60, 5);

    cout << "=== Teste Fsk ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//validado
void teste_camada_fisica::testa_8psk(){
    tx = new camada_fisica_transmissor(PORTADORA, PSK8, 5, 60, 5);
    rx = new camada_fisica_receptor(PORTADORA, PSK8, 5, 60, 5);

    cout << "=== Teste 8 - Psk ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}

//vaidado
void teste_camada_fisica::testa_32qam(){
    tx = new camada_fisica_transmissor(PORTADORA, QAM32, 5, 60, 5);
    rx = new camada_fisica_receptor(PORTADORA, QAM32, 5, 60, 5);

    cout << "=== Teste 32 qam ===\n" << endl;
    cout << "Sequencia de bits testada:\n";
    for(auto b : bit_stream) cout << b << ' ';
    cout << '\n';

    tx->set_bit_beacon(this->bit_stream);
    tx->modula_sinal();
    
    cout << "Sinal eletrico modulado em tx:\n";
    for(auto s : tx->get_modulated_signal()) cout << s << ' ';
    cout << '\n';
    
    rx->set_modulated_signal(tx->get_modulated_signal());
    rx->demodula_sinal();

    cout << "Sequencia de bits demodulada em rx:\n";
    for(auto b : rx->get_bit_beacon()) cout << b << ' ';
    cout << '\n' << endl;

    delete tx;
    delete rx;
}