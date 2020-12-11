/*
 ============================================================================
 Name        : TP3.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bcc/bcc.h>
#include <math.h>
#include "windows-producer.h"

#define windows_max_number 5

uint32_t g_nb_interruptions;
Windows_producer wp;
float img_buffer[windows_max_number * 36];
float fluxpondere[windows_max_number][10];

void interrupt_handler();
/**
* Enable an interrupt and set the handler.
* Used the value of the macro INTERRUPT_MASK_REGISTER.
* @param irq number of the interrupt to activate
* @param handler function to call when the interrupt \a irq is triggered
*/
void activate_interrupt(uint32_t irq, void* handler) ;
/**
* Disable an interrupt.
* Used the value of the macro INTERRUPT_MASK_REGISTER
* @param irq number of interrupt to disable
*/
void disable_interrupt(uint32_t irq) ;
/**
* Trigger the interrupt in parameter.
* Used the value of the macro INTERRUPT_FORCE_REGISTER
* @param irq number of interruption to trigger
*/
void force_interrupt(uint32_t irq) ;
void interrupt_handler_timer();
void interrupt_handler_image();

int32_t get_elapsed_time();

/**
* Configure les registres d'un timer pour qu'il déclenche une interruption
* après reload_value ticks et se recharge automatiquement avec la même période.
* @param timer_counter_register pointeur sur le registre de compteur du timer
* @param period nombre tick entre deux déclenchement du timer
*/
void start_timer(uint32_t* timer_counter_register, uint32_t period);


#define INTERRUPT_MASK_REGISTER 0x80000240
#define INTERRUPT_FORCE_REGISTER 0x80000208
#define TIMER_COUNTER_REGISTER 0x80000320
#define INTERRUPT_NUMBER 16
volatile uint32_t* interrupt_mask_register = (uint32_t*)INTERRUPT_MASK_REGISTER;
volatile uint32_t* interrupt_force_register = (uint32_t*)INTERRUPT_FORCE_REGISTER;
int32_t tab_temps[100];
int pret = 0;


double calculFluxPondere(float window[], float mask[]) {
	double res = 0;
	uint8_t xi = 0;
	uint8_t yi = 0;
	// --- parcoure window
	for (xi = 0; xi < 6; xi++) {
		for (yi = 0; yi < 6; yi++) {
			// somme les pixels, pondérés du facteur de mask
			res += window[xi * 6 + yi] * mask[xi * 6 + yi];
		}
	}
	return res;
}

/* Question 1 */
void put1bit(volatile uint32_t* adr, uint32_t irq) {
	* adr = * adr | (1 << irq);
}


void remove1bit(volatile uint32_t* adr, uint32_t irq) {
	* adr = * adr & ~(1 << irq);
}
/*
int testQuest1(){
	if(*interrupt_mask_register != 0) return 1;
	enable_interrupt(interrupt_mask_register, 1);
	if(*interrupt_mask_register != 2) return 2;
	enable_interrupt(interrupt_mask_register, 16);
	if(*interrupt_mask_register != 2) return 3;
	disable_interrupt(interrupt_mask_register, 1);
	if(*interrupt_mask_register != 0) return 4;
	return 0;
}*/

void testQuest2(){
	uint32_t compteur_test = 0;
	activate_interrupt(9, interrupt_handler);
	while ((g_nb_interruptions == 0) && (compteur_test < 100)) {
	compteur_test++;
	force_interrupt(9);
	}
	disable_interrupt(9) ;
	force_interrupt(9) ;
	printf("compteur test : %ld\nbn_interrupt : %ld",compteur_test,g_nb_interruptions);
}

void break_simu(void){
	return;
}

int main(void) {
	/*printf("%d\n",testQuest1());
	if(testQuest1()){
		puts ("Test non valide");
	} else {
		puts ("Test valide");
	}*/

	break_simu();
	testQuest2();
	break_simu();

	/* Question TIMER
	break_simu();
	activate_interrupt(9, interrupt_handler);
	start_timer((uint32_t *)TIMER_COUNTER_REGISTER, 5000);
	break_simu();
	while (g_nb_interruptions < 100) {}
	int smallest,biggest,mean,ecart,S1,S2,V;
	smallest = tab_temps[0];
	S1 = 0;
	S2 = 0;
	biggest = tab_temps[0];

	for (int i=0;i<100;i++){
	         S1 = S1+tab_temps[i];
	         S2 = S2+tab_temps[i]*tab_temps[i];
	         if(tab_temps[i] < smallest) smallest = tab_temps[i];
	         if(tab_temps[i] > biggest) biggest = tab_temps[i];
	     }
	     mean = S1/100;
	     V = S2/100-mean*mean;
	printf("moyenne : %d\nplus petit : %d\nplus grand : %d\nEcart type : %d\n",mean,smallest,biggest,ecart);
	break_simu();

	// FIN Question TIMER
	*/

	/*Question 4
	activate_interrupt(9, interrupt_handler_timer);
	activate_interrupt(10, interrupt_handler_image);
	start_timer((uint32_t *)TIMER_COUNTER_REGISTER, 100000);
	init(&wp, img_buffer, windows_max_number);
	enable_irq(&wp, 10);
	while(g_nb_interruptions <= 10){
		if(pret == 1){
			printf("\t\\hline\n\t\\multicolumn{1}{|l|}{%d}",(g_nb_interruptions-1) * 100);
			for(int i = 0; i < windows_max_number;i++){
				printf(" & %lf ",calculFluxPondere(&img_buffer[i*36], get_mask(&wp, i)));
				//printf("etoile %d : flux : %lf\n",i+1,calculFluxPondere(&img_buffer[i*36], get_mask(&wp, i)));
			}
			printf("\\\\\n");
			//printf("\n\n%d\n\n",g_nb_interruptions);
		pret = 0;
		}
	}
	*/

	return EXIT_SUCCESS;
}


void interrupt_handler() {
	g_nb_interruptions++;
}

void interrupt_handler_timer() {
	break_simu();
	produce_images(&wp);
	g_nb_interruptions++;
}

void interrupt_handler_image() {
	pret = 1;
}

/**
* Enable an interrupt and set the handler.
* Used the value of the macro INTERRUPT_MASK_REGISTER.
* @param irq number of the interrupt to activate
* @param handler function to call when the interrupt \a irq is triggered
*/
void activate_interrupt(uint32_t irq, void* handler){
	if(irq >= INTERRUPT_NUMBER) return;
	put1bit((uint32_t *)INTERRUPT_MASK_REGISTER,irq);
	//bcc_int_unmask(irq);
	bcc_isr_register(irq,handler,NULL);
}
/**
* Disable an interrupt.
* Used the value of the macro INTERRUPT_MASK_REGISTER
* @param irq number of interrupt to disable
*/
void disable_interrupt(uint32_t irq){
	//bcc_int_mask(irq);
	if(irq >= INTERRUPT_NUMBER) return;
	remove1bit((uint32_t *)INTERRUPT_MASK_REGISTER,irq);
}
/**
* Trigger the interrupt in parameter.
* Used the value of the macro INTERRUPT_FORCE_REGISTER
* @param irq number of interruption to trigger
*/
void force_interrupt(uint32_t irq){
	if(irq >= INTERRUPT_NUMBER) return;
	put1bit((uint32_t *)INTERRUPT_FORCE_REGISTER,irq);
	//bcc_int_force(irq);
}



/**
* Configure les registres d'un timer pour qu'il déclenche une interruption
* après reload_value ticks et se recharge automatiquement avec la même période.
* @param timer_counter_register pointeur sur le registre de compteur du timer
* @param period nombre tick entre deux déclenchement du timer
*/
void start_timer(uint32_t* timer_counter_register, uint32_t period){
	* timer_counter_register = period;
	* (uint32_t *)((uint32_t) timer_counter_register + 0x00000004) = period;
	put1bit((uint32_t *)((uint32_t) timer_counter_register + 0x00000008),0);
	put1bit((uint32_t *)((uint32_t) timer_counter_register + 0x00000008),1);
	put1bit((uint32_t *)((uint32_t) timer_counter_register + 0x00000008),3);
}
