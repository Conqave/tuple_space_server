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

char stack_thread_led[THREAD_STACKSIZE_MAIN];

static xtimer_ticks32_t time_elapsed;
static kernel_pid_t red_pid;

char stack_thread_blinking_green[THREAD_STACKSIZE_MAIN];
char stack_thread_red[THREAD_STACKSIZE_MAIN];

void *thread_red(void* arg){
    (void)arg;
    msg_t message_r;
    while(1){
    
    	msg_receive(&message_r);
    	uint32_t msg_timer = message_r.content.value;
    	printf("Red time elapsed: %ld us!\n\r", msg_timer);
    
    }
    /* tutaj napisz działanie wątku odbierającego wiadomości
    o czasie przytrzymania przycisku */
    /* funkcje: msg_receive */
    /* struktury: msg_t */

    return NULL;
}

static unsigned user_button_pressed_time = 0;

static void user_button_callback(void *arg){

	kernel_pid_t* red_pid_ptr;
	red_pid_ptr = (kernel_pid_t*)arg;
	static xtimer_ticks32_t begin_time;
	xtimer_ticks32_t end_time, time_elapsed;
    /* tak zmodyfikuj tę funkcję, żeby czas przytrzymania przycisku
    przesyłała do czerwonego wątku */
    /* funkcje: msg_send */
    /* struktury: msg_t */
    unsigned short button_pressed = gpio_read(USER_BUTTON);

    if(button_pressed == 0){
        end_time = xtimer_now();
        time_elapsed = xtimer_diff(end_time, begin_time);
        msg_t message_s;
        message_s.content.value = (uint32_t) time_elapsed.ticks32;
        msg_send(&message_s, *red_pid_ptr);
        button_pressed = 1;
    } else {
        begin_time = xtimer_now();
    }
}

char stack_thread_blinking_green[THREAD_STACKSIZE_MAIN];

void *thread_blinking_green(void* arg){
	(void) arg;
    /*z tego wątku usuń funkcjonalności związane z drukowaniem
    czasu przytrzymania przycisku */
    
    xtimer_ticks32_t last_wakeup = xtimer_now();
    GREEN_LED_ON;
    while(1){
    	last_wakeup = xtimer_now();
        GREEN_LED_TOGGLE;
        xtimer_periodic_wakeup(&last_wakeup, GREEN_LED_PERIOD);
        
    }    
}

int main(void)
{
    thread_create(stack_thread_blinking_green, sizeof(stack_thread_blinking_green),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                            thread_blinking_green, (void*)&user_button_pressed_time, "green");
    gpio_init_int(USER_BUTTON, GPIO_IN_PU, GPIO_BOTH, user_button_callback, (void*)&red_pid);
    /* tutaj napisz uruchomienie czerwonego wątku, przekaż pid tego wątku do user_button_callback
    poprzez callback argument lub zmienną globalną*/
    red_pid = thread_create(stack_thread_red, sizeof(stack_thread_red), THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST, thread_red,  (void*)&time_elapsed, "green");
    /* zmodyfikuj callback w przerwaniu generowanym przez przycisk */


    /* jeśli wykonanie kodu dotrze do tego miejsca,
    zmienne zdefiniowane w main() przestaną być dostępne */
    return 0;
}
