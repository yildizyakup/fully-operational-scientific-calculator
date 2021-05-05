#ifndef BSP_H_
#define BSP_H_

#include "stm32g0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*Common API function for Nucleo STM32G031 board*/
void BSP_system_init(void);
void delay(uint32_t);
void timer1(void);
void timer2(void);

//Functions
void BSP_calculation(int, int, int, int, int, int, int, int);
void BSP_set_values(void);
void BSP_number_control(void);
void BSP_scientific_function(float);
void BSP_displayNumber2_init(void);
void BSP_variables_init(void);
int numberOfDigits(float);

//SSD related functions
void BSP_ssd_init(void);
void BSP_ssd_set(int number);
void BSP_ssd_clear(void);
void BSP_ssd_display(int);
void BSP_ssd_Digit_n_ON(int);
void BSP_ssd_Digit_n_OFF(int);
void BSP_ssd_Digit_all_OFF(void);
void BSP_ssd_final_number(void);
void BSP_ssd_enter(void);
void BSP_ssd_ID_display(void);
void BSP_ssd_overflow_display(void);
void BSP_ssd_invalid_display(void);


//KEYPAD related functions
void BSP_keypad_init(void);
void BSP_keypad_set(void);
void BSP_keypad_clear(void);
void BSP_keypad_press(int, float);

#endif
