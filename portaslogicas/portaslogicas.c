#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "func/oled.h"


// valores dos pinos utilizados
#define EixoX 27
#define ButtonA 5
#define ButtonB 6
#define I2C_SDA 14
#define I2C_SCL 15
#define LEDR 13
#define LEDG 11
#define LEDB 12

unsigned int saida[4] = {0,0,0,0};

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

void seletor(unsigned int pos_atual, struct render_area frame_area){
    switch(pos_atual){
        case 0: //AND
            saida[0]=0;
            saida[1]=0;
            saida[2]=0;
            saida[3]=1;
            OLED_AND(frame_area);
            break;
        case 1: //OR
            saida[0]=0;
            saida[1]=1;
            saida[2]=1;
            saida[3]=1;
            OLED_OR(frame_area);
            break;
        case 2: //NOT
            saida[0]=1;
            saida[1]=1;
            saida[2]=0;
            saida[3]=0;
            OLED_NOT(frame_area);
            break;
        case 3: //NAND
            saida[0]=1;
            saida[1]=1;
            saida[2]=1;
            saida[3]=0;
            OLED_NAND(frame_area);
            break;
        case 4: //NOR
            saida[0]=1;
            saida[1]=0;
            saida[2]=0;
            saida[3]=0;
            OLED_NOR(frame_area);
            break;
        case 5:
            saida[0]=0;
            saida[1]=1;
            saida[2]=1;
            saida[3]=0;
            OLED_XOR(frame_area);
            break;
        case 6:
            saida[0]=1;
            saida[1]=0;
            saida[2]=0;
            saida[3]=1;
            OLED_XNOR(frame_area);
            break;
    }
}

int main(){

    unsigned int adc_x = 0, pos_atual = 0, flag=0;
    inicializacao();

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

	//Zerando o display para garantir que nada esteja sendo exibido na tela OLED
    zeraDisplay(frame_area);

    while (true){
        seletor(pos_atual, frame_area);
        do{
            if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
                if(saida[0]==1){
                    led_true();
                }
                else{
                    led_false();
                }
            }
            else if(gpio_get(ButtonA)==0){ //apenas A apertado
                if(saida[1]==1){
                    led_true();
                }
                else{
                    led_false();
                }
            }
            else if(gpio_get(ButtonB)==0){ //apenas B apertado
                if(saida[2]==1){
                    led_true();
                }
                else{
                    led_false();
                }
            }
            else{ //ambos soltos
                if(saida[3]==1){
                    led_true();
                }
                else{
                    led_false();
                }
            }

            adc_select_input(1);
            adc_x = adc_read();
            if (adc_x < 50){
                if (pos_atual != 0){ // inserir menor valor possível
                    pos_atual--;
                }
                else{
                    pos_atual=6;
                }
            }
            else if (adc_x > 4050){
                if (pos_atual != 6){ // inserir maior valor possível
                    pos_atual++;
                }
                else{
                    pos_atual=0;
                }
            }
            sleep_ms(100);
            // Condicionais para a saída do do-while de navegação
        } while (adc_x > 50 && adc_x < 4050);
    }
}
