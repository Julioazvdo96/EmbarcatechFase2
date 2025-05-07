#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "inc/ssd1306.h"
#include "inc/ws2812b.pio.h"
#include <string.h>

#define EixoY 26
#define SW_JOY 22

#define LEDR 13
#define LEDG 11
#define LEDB 12

#define I2C_SDA 14
#define I2C_SCL 15

#define MATRIX_PIN 7
#define MATRIX_COUNT 25 

uint matrix[4] = {0,0,0,0}; 


// Função para atualizar o display OLED
void atualizarDisplay(uint8_t *ssd, struct render_area *areaRender, uint pos_atual) {
    memset(ssd, 0, ssd1306_buffer_length);
    
    char textoEntrada[32], apresentacaoEntrada[32];
    
    if (pos_atual==0) {
        snprintf(apresentacaoEntrada, sizeof(apresentacaoEntrada), "Porteiro");
        snprintf(textoEntrada, sizeof(textoEntrada), "PT:\n  %d", matrix[0]);
    } 
    else if (pos_atual==1) {
        snprintf(apresentacaoEntrada, sizeof(apresentacaoEntrada), "Dia Valido");
        snprintf(textoEntrada, sizeof(textoEntrada), "DI:\n  %d", matrix[1]);
    } 
    else if (pos_atual==2){
        snprintf(apresentacaoEntrada, sizeof(apresentacaoEntrada), "Hora Valida");
        snprintf(textoEntrada, sizeof(textoEntrada), "HO:\n  %d", matrix[2]);
    }
    else{
        snprintf(apresentacaoEntrada, sizeof(apresentacaoEntrada), "Autenticacao ok");
        snprintf(textoEntrada, sizeof(textoEntrada), "GR:\n  %d", matrix[3]);
    }

    
    ssd1306_draw_string(ssd, 5, 25, apresentacaoEntrada);
    ssd1306_draw_string(ssd, 40, 45, textoEntrada);
    render_on_display(ssd, areaRender);
}

void zeraDisplay(struct render_area frame_area){
	uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);
}

// Definição de pixel GRB
struct pixel_t {
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
   };
   typedef struct pixel_t pixel_t;
   typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.
   
   // Declaração do buffer de pixels que formam a matrix.
   npLED_t leds[MATRIX_COUNT];
   
   // Variáveis para uso da máquina PIO.
   PIO np_pio;
   uint sm;
   
   /**
   * Inicializa a máquina PIO para controle da matrix de LEDs.
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
    adc_gpio_init(EixoY);

    // Inicialização dos LEDs
    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);
    gpio_init(SW_JOY);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, GPIO_OUT);
    gpio_set_dir(LEDG, GPIO_OUT);
    gpio_set_dir(LEDB, GPIO_OUT);
    gpio_set_dir(SW_JOY, GPIO_IN); 
    
    gpio_pull_up(SW_JOY);

    npInit(MATRIX_PIN);
    npClear();
}
//funções de manipulação do LED RGB
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
    uint adc_y, pos_atual=0, flag_do=0,plus_one=0;
    inicializacao();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    uint8_t ssd[ssd1306_buffer_length];

	//Zerando o display para garantir que nada esteja sendo exibido na tela OLED
    zeraDisplay(frame_area);

    while (true) {
        //Lógica de seleção e impressão das cores na matriz de LED
        for(int i=0;i<4;i++){
            plus_one=i+1;
            if(matrix[i]==0){
                npSetLED(plus_one, 10, 0, 0);
            }
            else{
                npSetLED(plus_one, 0, 10, 0);
            }
            plus_one=0;
        }

        npWrite();

        //Lógica de seleção do LED RGB que representará a saída CT 
        if(matrix[0]==0 || (matrix[1]==1 && matrix[2]==1 && matrix[3]==1)){
            led_true();
        }
        else{
            led_false();
        }
        //Lógica de navegação e escolha de valores utilizando o joystick e o seu botão
        do{
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
            else if (gpio_get(SW_JOY)==0){
                sleep_ms(100);
                if(matrix[pos_atual]==0){
                    matrix[pos_atual]=1;
                }
                else{
                    matrix[pos_atual]=0;
                }
                flag_do=1;
            }
            
            sleep_ms(100);
            // Condicionais para a saída do do-while de navegação

            atualizarDisplay(ssd, &frame_area, pos_atual);
        } while (flag_do!=1);
        flag_do=0;
    }
}
