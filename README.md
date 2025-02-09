# Comunicação Serial e Controle de Hardware com BitDogLab
Yasmin Damasceno Cruz - TIC370101610  
Unidade 4 - Capítulo 6

Este projeto explora o uso de interfaces de comunicação serial no RP2040, utilizando a placa de desenvolvimento BitDogLab. O foco está na manipulação de LEDs comuns e endereçáveis, botões com interrupções e debounce, além da exibição de caracteres em um display OLED via I2C. 

O código é escrito em C e executado no Raspberry Pi Pico W, integrando hardware e software para proporcionar uma experiência prática no uso de UART e I2C. A simulação pode ser realizada na ferramenta Wokwi integrada ao VS Code.

## Componentes Utilizados

- 1x Placa de desenvolvimento BitDogLab
- 1x Matriz 5x5 de LEDs WS2812 (GPIO 7)
- 1x LED RGB (GPIOs 11, 12 e 13)
- 1x Botão A (GPIO 5)
- 1x Botão B (GPIO 6)
- 1x Display SSD1306 via I2C (GPIOs 14 e 15)

## Configuração Inicial

1. Clone o repositório para sua máquina local:
   ```bash
   git clone <https://github.com/YasminCruz001/Comunicacao_Serial.git>
   ```
2. Abra o projeto no VS Code.
3. Utilize o Wokwi para simular os componentes conectados à placa BitDogLab.

## Funcionalidades do Projeto

### 1. Modificação da Biblioteca `font.h`
- Adição de caracteres minúsculos personalizados para exibição no display SSD1306.

### 2. Entrada de Caracteres via UART
- O usuário pode inserir caracteres pelo Serial Monitor do VS Code.
- Cada caractere digitado é exibido no display SSD1306.
- Se um número entre 0 e 9 for digitado, um símbolo correspondente será exibido na matriz de LEDs WS2812.

### 3. Interação com o Botão A
- Pressionar o Botão A alterna o estado do LED RGB Verde (ligado/desligado).
- A operação é registrada de duas formas:
  - Uma mensagem informativa no display SSD1306.
  - Um texto descritivo enviado ao Serial Monitor.

### 4. Interação com o Botão B
- Pressionar o Botão B alterna o estado do LED RGB Azul (ligado/desligado).
- A operação é registrada de duas formas:
  - Uma mensagem informativa no display SSD1306.
  - Um texto descritivo enviado ao Serial Monitor.

## Requisitos do Projeto

- **Uso de interrupções**: Todas as funcionalidades dos botões são implementadas utilizando IRQs.
- **Debouncing**: Implementado via software para evitar leituras incorretas.
- **Controle de LEDs**: Inclui LEDs comuns e WS2812, demonstrando domínio sobre diferentes formas de controle.
- **Utilização do Display SSD1306**: Exibe caracteres maiúsculos e minúsculos, reforçando o entendimento do protocolo I2C.
- **Envio de informações via UART**: Demonstra compreensão da comunicação serial.
- **Código bem estruturado**: Organizado e comentado para facilitar a manutenção e entendimento.

## Execução

1. Compile e carregue o código na placa BitDogLab.
2. Abra o Serial Monitor no VS Code para interagir com o sistema.
3. Pressione os botões e observe a resposta no display e no Serial Monitor.
4. Insira caracteres pelo Serial Monitor e veja-os aparecer no display SSD1306.
5. Digite números para exibir símbolos na matriz 5x5 de LEDs WS2812.
