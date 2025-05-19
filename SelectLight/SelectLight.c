// Retirado de https://github.com/BitDogLab/BitDogLab-C/blob/main/blink/blink.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEDR 13
#define LEDG 11
#define LEDB 12

#define BUTTON_A 5


void led_rgb_put(bool r, bool g, bool b) {
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}

int main() {
    int a=0;
    stdio_init_all();

    gpio_init(LEDR);
    gpio_init(LEDG);
    gpio_init(LEDB);
    gpio_init(BUTTON_A);

    gpio_set_slew_rate(LEDR, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDG, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LEDB, GPIO_SLEW_RATE_SLOW);

    gpio_set_dir(LEDR, GPIO_OUT);
    gpio_set_dir(LEDG, GPIO_OUT);
    gpio_set_dir(LEDB, GPIO_OUT);
    
    gpio_set_dir(BUTTON_A, GPIO_IN);

    gpio_pull_up(BUTTON_A);


    while (true) {
        printf("a = %d", a);
        if(gpio_get(BUTTON_A)==0){
            a++;
            printf("botão pressionado");
        }
        else if(a==0){
            led_rgb_put(1,0,0);
            
            printf("vermelho");
        }
        else if(a==1){
            led_rgb_put(0,1,0);
            printf("verde");

        }
        else if(a==2){
            led_rgb_put(0,0,1);
            printf("azul");

        }
        else{
            a=0;
        }
    sleep_ms(200);
    }
}