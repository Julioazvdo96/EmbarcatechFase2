#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

// valores dos pinos utilizados
#define EixoX 27
#define ButtonA 5
#define ButtonB 6
#define I2C_SDA 14
#define I2C_SCL 15
#define LEDR 13
#define LEDG 11
#define LEDB 12

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

    // Inicialização do botão do joystick
    gpio_init(ButtonA);
    gpio_set_dir(ButtonA, GPIO_IN);
    gpio_pull_up(ButtonA);
    gpio_init(ButtonB);
    gpio_set_dir(ButtonB, GPIO_IN);
    gpio_pull_up(ButtonB);

    // Inicialização do ADC
    adc_init();
    adc_gpio_init(EixoX);

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

void AND(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_true();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_false();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_false();
        }
        else{ //ambos soltos
            led_false();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void OR(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_true();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_true();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_true();
        }
        else{ //ambos soltos
            led_false();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void NOT(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0){ // condicional dos dois apertados
            led_false();
        }
        else{ //ambos soltos
            led_true();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void NAND(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_false();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_true();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_true();
        }
        else{ //ambos soltos
            led_true();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void NOR(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_false();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_false();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_false();
        }
        else{ //ambos soltos
            led_true();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void XOR(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_false();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_true();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_true();
        }
        else{ //ambos soltos
            led_false();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

void XNOR(unsigned int adc_x){
    do{
        if(gpio_get(ButtonA)==0 && gpio_get(ButtonB)==0){ // condicional dos dois apertados
            led_true();
        }
        else if(gpio_get(ButtonA)==0){ //apenas A apertado
            led_false();
        }
        else if(gpio_get(ButtonB)==0){ //apenas B apertado
            led_false();
        }
        else{ //ambos soltos
            led_true();
        }
        adc_select_input(1);
        adc_x = adc_read();
    }while(adc_x > 50 && adc_x < 4050);
}

int main(){

    unsigned int adc_x = 0, pos_atual = 0;
    inicializacao();

    while (true){
        printf("pos atual = %d\n", pos_atual);
        switch(pos_atual){
            case 0: 
                AND(adc_x);
                //func menu
                break;
            case 1:
                OR(adc_x);
                //func menu
                break;
            case 2:
                NOT(adc_x);
                //func menu
                break;
            case 3:
                NAND(adc_x);
                //func menu
                break;
            case 4:
                NOR(adc_x);
                //func menu
                break;
            case 5:
                XOR(adc_x);
                //func menu
                break;
            case 6:
                XNOR(adc_x);
                //func menu
                break;
        }

        // do-while que coleta informação do ADC do eixo x do joystick
        do{
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
