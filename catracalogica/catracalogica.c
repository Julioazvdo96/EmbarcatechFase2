#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "inc/ssd1306.h"

#define EixoX 27
#define EixoY 26

#define LEDR 13
#define LEDG 11
#define LEDB 12

#define I2C_SDA 14
#define I2C_SCL 15

uint matrix[4] = {0,0,0,0}; 

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
}

int main(){
    uint adc_x, adc_y, pos_atual=0;
    inicializacao();
    
    while (true) {
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
            sleep_ms(100);
            // Condicionais para a saída do do-while de navegação
            printf("adc_x = %d adc_y = %d\n", adc_x,adc_y);
            printf("matrix[%d]=%d\n", pos_atual,matrix[pos_atual]);
            sleep_ms(1000);
        } while (adc_x > 50 && adc_x < 4050);
    }
}
