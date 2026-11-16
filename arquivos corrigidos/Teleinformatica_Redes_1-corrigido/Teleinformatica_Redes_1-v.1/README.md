# Simulador de Camadas Física e Enlace - Teleinformatica_Redes_1

Este projeto consiste em uma simulação completa de um sistema de comunicação digital, abrangendo desde a interface de aplicação (chat) até a propagação de sinais pelo meio físico, passando pelas camadas de enlace e física. O objetivo principal é demonstrar de forma prática o funcionamento de protocolos de enquadramento, modulação e detecção/correção de erros sob condições reais de ruído.   
# Arquitetura e Modelagem do Sistema

O sistema foi desenvolvido utilizando o Paradigma Orientado a Objetos (POO) e estruturado em blocos modulares que refletem diretamente os modelos de referência de redes:
1) Camada de Aplicação (Chat): Interface responsável pela entrada e saída de texto, convertendo as mensagens dos usuários em fluxos brutos de bits/bytes.   
2) Camada de Enlace: Responsável pelo enquadramento dos dados (delimitando o início e fim dos pacotes) e pela aplicação de algoritmos de Detecção e Correção de Erros (EDC).  
3) Camada Física: Realiza a codificação banda-base (ex: NRZ) convertendo sequências de bits em níveis de tensão elétrica ($V$), além de lidar com a modulação.   
4) Meio de Comunicação (Canal): Uma interface intermediária que simula um canal de transmissão real. Ele intercepta o sinal elétrico gerado pelo transmissor, injeta ruído gaussiano (baseado em uma distribuição normal n(x, $\sigma$) e o entrega corrompido ao receptor para testes de robustez.   

# Funcionalidades Principais
1) Chat da aplicação : Aplicação que provê comunicação na forma de texto (sequências de caracteres) entre Transmissor (TX) e Receptor (RX)
2) Configuração Dinâmica: Painel de controle para ajustar o tamanho máximo de quadros, tipos de enquadramento, esquemas de modulação e parâmetros do ruído no canal. 
3) Simulação Bidirecional: Objetos independentes para Transmissor (TX) e Receptor (RX) operando de forma integrada ou concorrente.
