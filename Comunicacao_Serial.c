#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"
#include "ssd1306.h"

// Definição dos pinos dos LEDs RGB
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

// Número total de LEDs na matriz
#define NUM_PIXELS 25

// Definição dos pinos dos botões e do barramento WS2812
#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define WS2812_PIN 7

// Definição da interface I2C e pinos correspondentes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C // Endereço do display SSD1306

uint32_t led_buffer[NUM_PIXELS]; // Buffer para armazenar os valores dos LEDs
ssd1306_t ssd; // Estrutura para o display OLED

// Estados dos LEDs com controle volátil 
volatile bool led_green_state = false;
volatile bool led_blue_state = false;

// Estados anteriores dos LEDs
volatile bool prev_led_green_state = false;
volatile bool prev_led_blue_state = false;

// Estruturas para armazenar informações sobre os LEDs
typedef struct
{
    uint pin;                   // Pino do LED
    bool state;                 // Estado atual do LED (ligado/desligado)
    const char *color_name;     // Nome da cor
    uint8_t r, g, b;            // Valores RGB da cor
} Led;

// Array contendo os LEDs RGB
Led leds[] = {
    {LED_GREEN, false, "Verde", 0, 255, 0},
    {LED_BLUE, false, "Azul", 0, 0, 255},
    {LED_RED, false, "Vermelho", 255, 0, 0}};

// Inicializa todos os LEDs RGB
void init_rgb_leds()
{
    for (int i = 0; i < 3; i++)
    {
        gpio_init(leds[i].pin);
        gpio_set_dir(leds[i].pin, GPIO_OUT);
    }
}

// Matriz de números representados por LEDs na matriz 5x5
const int numeros[10][NUM_PIXELS] = {

// Representação dos números de 0 a 9 na matriz 5x5
   // 0
  {0, 1, 1, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0},

   // 1
  {0, 1, 1, 0, 0,
   0, 0, 1, 0, 0,
   0, 0, 1, 0, 0,
   0, 0, 1, 0, 0,
   0, 1, 1, 1, 0},

   // 2
  {0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 1, 1, 1, 0,
   0, 1, 0, 0, 0,
   0, 1, 1, 1, 0},

   // 3
  {0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 1, 1, 1, 0},

   // 4
  {0, 1, 0, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 0, 0, 1, 0},

   // 5
  {0, 1, 1, 1, 0,
   0, 1, 0, 0, 0,
   0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 1, 1, 1, 0},

   // 6
  {0, 1, 0, 0, 0,
   0, 1, 0, 0, 0,
   0, 1, 1, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0},

   // 7
  {0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 0, 0, 1, 0,
   0, 0, 0, 1, 0,
   0, 0, 0, 1, 0},

   // 8
  {0, 1, 1, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0},

   // 9
  {0, 1, 1, 1, 0,
   0, 1, 0, 1, 0,
   0, 1, 1, 1, 0,
   0, 0, 0, 1, 0,
   0, 0, 0, 1, 0},
   };
   
// Função para converter valores RGB para um formato de 32 bits compatível com WS2812
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

int ordem_fisica[NUM_PIXELS] =
    {
        24, 23, 22, 21, 20,
        15, 16, 17, 18, 19,
        14, 13, 12, 11, 10,
        5, 6, 7, 8, 9,
        4, 3, 2, 1, 0};

// Função para desenhar um número na matriz de LEDs
void desenhar_numero(int num, PIO pio, uint sm)
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        int indice_fisico = ordem_fisica[i];
        led_buffer[i] = numeros[num][indice_fisico] ? matrix_rgb(1.0, 0.0, 0.0) : matrix_rgb(0.0, 0.0, 0.0);
    }
        
    // Envia os dados da matriz para os LEDs WS2812
    for (int i = 0; i < NUM_PIXELS; i++)
    {
                pio_sm_put_blocking(pio, sm, led_buffer[i]);
    }
}
        
// Inicializa os pinos dos LEDs e botões
void init_pins()
{
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, false);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, false);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, false);

    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
}

// Função para lidar com debounce dos botões
void debounce_button(uint gpio, volatile bool *state, absolute_time_t *last_press_time)
{
    absolute_time_t now = get_absolute_time();
    // Verifica se o tempo de debounce passou (50ms)
    if (absolute_time_diff_us(*last_press_time, now) > 50000)
    {
        *last_press_time = now;
        *state = !(*state); // Alterna o estado
    }
}

// Função para atualizar o estado do LED e exibir no display OLED
void atualizar_led_e_display(ssd1306_t *ssd, int led_pin, const char *led_name, const char *btn_name, bool led_state)
{
    gpio_put(led_pin, led_state); // Atualiza o estado do LED

    // Exibe no display qual botão foi pressionado
    char message[32];
    ssd1306_fill(ssd, false);
    sprintf(message, "Botao %s", btn_name);
    ssd1306_draw_string(ssd, message, 10, 10);

    // Exibe o estado do LED correspondente
    sprintf(message, "LED %s/n: %s", led_name, led_state ? "Ligado" : "Desligado");
    ssd1306_draw_string(ssd, message, 10, 20);

    ssd1306_send_data(ssd);
}

// Variáveis para armazenar o tempo da última pressão dos botões
absolute_time_t last_press_time_A = {0};
absolute_time_t last_press_time_B = {0};

void btn_irq_handler(uint gpio, uint32_t events)
{
    absolute_time_t now = get_absolute_time();

    if (gpio == BTN_A_PIN)
    {
        if (absolute_time_diff_us(last_press_time_A, now) > 150000)
        {
            last_press_time_A = now;

            led_green_state = !led_green_state; // Alterna o estado
            gpio_put(LED_GREEN, led_green_state);

            // Mensagem única no Serial Monitor
            printf("Botao A pressionado. LED Verde %s/n", led_green_state ? "Ligado" : "Desligado");

            // Atualiza o display sem imprimir novamente no Serial Monitor
            atualizar_led_e_display(&ssd, LED_GREEN, "Verde", "A", led_green_state);
        }
    }
    else if (gpio == BTN_B_PIN)
    {
        if (absolute_time_diff_us(last_press_time_B, now) > 150000)
        {
            last_press_time_B = now;

            led_blue_state = !led_blue_state; // Alterna o estado
            gpio_put(LED_BLUE, led_blue_state);

            // Mensagem única no Serial Monitor
            printf("Botao B pressionado. LED Azul %s/n", led_blue_state ? "Ligado" : "Desligado");

            // Atualiza o display sem imprimir novamente no Serial Monitor
            atualizar_led_e_display(&ssd, LED_BLUE, "Azul", "B", led_blue_state);
        }
    }
}


// Configuração e inicialização do display OLED SSD1306
void init_display() {
    // Inicializa o barramento I2C
    i2c_init(I2C_PORT, 400 * 1000);             // Configura a taxa de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL
    gpio_pull_up(I2C_SDA);                      // Habilita pull-up para SDA
    gpio_pull_up(I2C_SCL);                      // Habilita pull-up para SCL

    // Inicializa o SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);       // Configuração do display
    ssd1306_send_data(&ssd);    // Envia dados para o display
    ssd1306_fill(&ssd, false);  // Preenche com cor (falso para limpar)
    ssd1306_send_data(&ssd);    // Atualiza a tela
}

// Interrupção para lidar com eventos da UART
void on_uart_rx()
{
    uart_init(uart0, 115200);               // Inicializa a UART0 com baud rate de 115200
    gpio_set_function(0, GPIO_FUNC_UART);   // Configura pino 0 como TX
    gpio_set_function(1, GPIO_FUNC_UART);   // Configura pino 1 como RX
    uart_set_fifo_enabled(uart0, true);
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    uart_set_irq_enables(uart0, true, false);
    irq_set_enabled(UART0_IRQ, true);

    
         while (uart_is_readable(uart0))
         {
             char c = uart_getc(uart0);
             uart_putc(uart0, c);
         }
}

int main()
{
    stdio_init_all();
    init_pins();

    PIO pio = pio0; // Define a PIO que será usada para os LEDs WS2812
    uint16_t i;
    uint32_t valor_led;
    double r = 0.0, b = 1.0, g = 0.0;

    // Configura o clock do sistema para 128 MHz
    bool ok = set_sys_clock_khz(128000, false);
    if (ok)
    {
        printf("Clock set to %ld\n", clock_get_hz(clk_sys));
    }

    // Configuração da PIO para os LEDs WS281
    uint offset = pio_add_program(pio, &ws2812_program);
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    init_display(&ssd); // Inicializa o display OLED SSD1306

    printf("Digite um valor para exibir no display\n");

    // Configura interrupções nos botões
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, btn_irq_handler);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true, btn_irq_handler);


    while (true)
    {
        printf("Número: ");
        char input = getchar(); // Lê um caractere da entrada padrão (teclado)
      
    // Exibe o valor no display
    char message[2] = {input, '\0'};                // Cria uma string com um único caractere
    ssd1306_fill(&ssd, false);                      // Limpa o display
    ssd1306_draw_string(&ssd, message, 20, 20);     // Desenha o caractere na tela
    ssd1306_send_data(&ssd);                        // Atualiza o display

    // Verifica se o caractere digitado é um número entre '0' e '9
    if (input >= '0' && input <= '9')
    {   
        int numero = input - '0'; // Converte o caractere numérico para um inteiro

        // Exibe o número na matriz de LEDs
        desenhar_numero(numero, pio, sm);
        printf("Exibindo o número %d na matriz de LEDs.\n", numero);
    }

    else
    {   
        // Se não for um número, apenas exibe o caractere no display
        printf("Caractere '%c' exibido no display.\n", input);
    }
      }
    
    }

