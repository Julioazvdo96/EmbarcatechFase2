#ifndef OLED_H
#define OLED_H

#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>

void zeraDisplay(struct render_area frame_area);  // Declaração da função zeraDisplay

void OLED_AND(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica AND
void OLED_OR(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica OR
void OLED_NOT(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica NOT
void OLED_NAND(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica NAND
void OLED_NOR(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica NOR
void OLED_XOR(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica XOR
void OLED_XNOR(struct render_area frame_area);       // Declaração da função que mostrará o símbolo da porta lógica XNOR

#endif 