/*
 * Copyright (C) 2021 Warsaw University of Technology
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lab
 * @{
 *
 * @file
 * @brief       mcu programming (RIOT OS) - threading and irq
 *
 * @author      Norbert Niderla <norbert.niderla@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"       /* board specific definitions */
#include "periph/gpio.h" /* gpio api */
#include "stm32l072xx.h" /* mcu specific definitions */

/* threading includes */
#include "thread.h"
#include "msg.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (1)
#if ENABLE_DEBUG
#include "debug.h"
#endif

/* button manipulation macro */
#define USER_BUTTON       (BTN_B1_PIN)

/* led manipulation macros */
#define RED_LED_OFF       (LED3_OFF)
#define RED_LED_ON        (LED3_ON)
#define RED_LED_TOGGLE    (LED3_TOGGLE)
#define BLUE_LED_OFF      (LED2_OFF)
#define BLUE_LED_ON       (LED2_ON)
#define BLUE_LED_TOGGLE   (LED2_TOGGLE)
#define GREEN_LED_OFF     (LED1_OFF)
#define GREEN_LED_ON      (LED1_ON)
#define GREEN_LED_TOGGLE  (LED1_TOGGLE)

/* leds period times (can be changed) */
#define RED_LED_PERIOD         (250000)
#define GREEN_LED_PERIOD     (250000)
#define BLUE_LED_PERIOD     (250000)

/* tutaj zadeklaruj zmienną globalną, ktora bedzie przechowywać czas
    przez który ostatnio przycisk był wciśnięty */

static xtimer_ticks32_t time_elapsed;

static void user_button_callback(void *arg){
    xtimer_ticks32_t* time_elapsed_ptr;
    time_elapsed_ptr = (xtimer_ticks32_t*)arg; 
    static xtimer_ticks32_t begin_time;
    xtimer_ticks32_t end_time;

   unsigned short button_pressed = gpio_read(USER_BUTTON);

    if(button_pressed){
        end_time = xtimer_now();
        *time_elapsed_ptr = xtimer_diff(end_time, begin_time);
    } else {
        begin_time = xtimer_now();
    }
}

char stack_thread_blinking_green[THREAD_STACKSIZE_MAIN];

void *thread_blinking_green(void* arg){

    /* napisz drukowanie czasu przycisniecia przycisku w tym watku */
    xtimer_ticks32_t* time_elapsed_ptr;
    time_elapsed_ptr = (xtimer_ticks32_t*)arg;
    xtimer_ticks32_t last_time_elapsed = xtimer_now();
    xtimer_ticks32_t last_wakeup = xtimer_now();
    GREEN_LED_ON;
    while(1){
       last_wakeup = xtimer_now();
        GREEN_LED_TOGGLE;
        if(time_elapsed_ptr->ticks32 != last_time_elapsed.ticks32){
         printf("Time elapsed: %ld us! \n\r", time_elapsed_ptr->ticks32);
	 last_time_elapsed = *time_elapsed_ptr;
    }
        xtimer_periodic_wakeup(&last_wakeup, GREEN_LED_PERIOD);
    }    

    return NULL;
}

int main(void)
{
    thread_create(stack_thread_blinking_green, sizeof(stack_thread_blinking_green),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                            thread_blinking_green, (void*)&time_elapsed, "green");

    gpio_init_int(USER_BUTTON, GPIO_IN_PU, GPIO_BOTH, user_button_callback, (void*)&time_elapsed);

    /* jeśli wykonanie kodu dotrze do tego miejsca,
    zmienne zdefiniowane w main() przestaną być dostępne */
    return 0;
}
