#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "inc/ws2812b.pio.h"

#define EixoX 27
#define EixoY 26

#define LEDR 13
#define LEDG 11
#define LEDB 12

#define I2C_SDA 14
#define I2C_SCL 15

#define MATRIX_PIN 7
#define MATRIX_COUNT 25 


uint matrix[4] = {0,0,0,0}; 

// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
   };
   typedef struct pixel_t pixel_t;
   typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.
   
   // Declaração do buffer de pixels que formam a matriz.
   npLED_t leds[MATRIX_COUNT];
   
   // Variáveis para uso da máquina PIO.
   PIO np_pio;
   uint sm;
   
   /**
   * Inicializa a máquina PIO para controle da matriz de LEDs.
   */
   void npInit(uint pin) {
   
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
   
    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0) {
      np_pio = pio1;
      sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }
   
    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
   
    // Limpa buffer de pixels.
    for (uint i = 0; i < MATRIX_COUNT; ++i) {
      leds[i].R = 0;
      leds[i].G = 0;
      leds[i].B = 0;
    }
   }
   
   /**
   * Atribui uma cor RGB a um LED.
   */
   void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
   }
   
   /**
   * Limpa o buffer de pixels.
   */
   void npClear() {
    for (uint i = 0; i < MATRIX_COUNT; ++i)
      npSetLED(i, 0, 0, 0);
   }
   
   /**
   * Escreve os dados do buffer nos LEDs.
   */
   void npWrite() {
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < MATRIX_COUNT; ++i) {
      pio_sm_put_blocking(np_pio, sm, leds[i].G);
      pio_sm_put_blocking(np_pio, sm, leds[i].R);
      pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
   }

void inicializacao(){
    stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Inicialização do ADC
    adc_init();
    adc_gpio_init(EixoX);
    adc_gpio_init(EixoY);

    // Inicialização dos LEDs
    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, GPIO_OUT);
    gpio_set_dir(LEDG, GPIO_OUT);
    gpio_set_dir(LEDB, GPIO_OUT);

    npInit(MATRIX_PIN);
    npClear();
}

void led_true() {
    gpio_put(LEDR, false);
    gpio_put(LEDG, true);
    gpio_put(LEDB, false);
}

void led_false() {
    gpio_put(LEDR, true);
    gpio_put(LEDG, false);
    gpio_put(LEDB, false);
}


int main(){
    uint adc_x, adc_y, pos_atual=0;
    inicializacao();

    while (true) {
        for(int i=0;i<4;i++){
            if(matrix[i]==0){
                npSetLED(i, 10, 0, 0);
            }
            else{
                npSetLED(i, 0, 10, 0);
            }
        }

        npWrite();

        if(matrix[3]==0 || (matrix[0]==1 && matrix[1]==1 && matrix[2]==1)){
            led_true();
        }
        else{
            led_false();
        }

        do{
            adc_select_input(1);
            adc_x = adc_read();
            adc_select_input(0);
            adc_y = adc_read();
            if (adc_y <50){
                if (pos_atual != 0){ // inserir menor valor possível
                    pos_atual--;
                }
                else{
                    pos_atual=3;
                }
            }
            else if (adc_y > 4050){
                if (pos_atual != 3){ // inserir maior valor possível
                    pos_atual++;
                }
                else{
                    pos_atual=0;
                }
            }
            else if (adc_x < 50){
                matrix[pos_atual]=0;
            }
            else if (adc_x > 4050){
                matrix[pos_atual]=1;
            }
            sleep_ms(300);
            // Condicionais para a saída do do-while de navegação
            printf("matrix[%d]=%d\n", pos_atual,matrix[pos_atual]);
            printf("adc_x = %d adc_y = %d\n", adc_x, adc_y);
        } while (adc_x > 50 && adc_x < 4050);
    }
}
