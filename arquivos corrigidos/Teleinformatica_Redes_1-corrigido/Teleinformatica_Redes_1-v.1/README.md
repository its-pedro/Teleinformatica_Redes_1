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

# Como rodar o simulador 
### Para Windows
1. Instale o MSYS2 (msys2.org) e abra o terminal **MSYS2 UCRT64**. Não use o PowerShell.
2. Instale as ferramentas (uma vez só):
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gtkmm3 mingw-w64-ucrt-x86_64-pkgconf make
```
 
3. Selecione no terminal a pasta do projeto baixado:
```bash
cd /c/Users/$USERNAME/Downloads/Teleinformatica_Redes_1-v.2
```
 
4. Compile e rode:
```bash
make gui
./simulador_gui.exe
```
 
### Para Linux (Ubuntu / Debian)
 
1. Instale as dependências (uma vez só):
```bash
sudo apt update
sudo apt install build-essential pkg-config libgtkmm-3.0-dev
```
 
2. Entre na pasta do projeto, compile e rode:
```bash
cd Teleinformatica_Redes_1-v.2
make gui
./simulador_gui
```
 
## Uso da interface
 
1. Na aba **Controle / Configuração**, escolha enquadramento, EDC, modulação e ruído.
2. Na aba **Transmissor (TX)**, digite a mensagem e clique em **Transmitir**.
3. Na aba **Receptor (RX)**, confira os bits, os quadros e a mensagem recebida.
## Outros comandos do Makefile
 
| Comando      | O que faz                                                                         |
|--------------|-----------------------------------------------------------------------------------|
| `make`       | compila o teste da camada física e o teste do simulador, sem precisar do gtkmm    |
| `make test`  | roda as 140 combinações de TX → canal → RX no terminal                            |
| `make clean` | apaga os arquivos compilados                                                      |
 
Caso haja erro de compilação, rode `make clean` e depois `make gui` de novo.
