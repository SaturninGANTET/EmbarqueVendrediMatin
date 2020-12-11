/*
 ============================================================================
 Name        : TP2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "tp2-algo.h"
#include <bcc/bcc.h>

uint32_t break_simu(uint32_t break_id, const char * break_name/*, int32_t time*/) __attribute__((optimize(0)));
//int32_t get_elapsed_time();

float window[] = {
2.4, 52.38, 2.36, 2.34, 82.32, 92.3,
12.4, 1710.38, 3716.36, 3558.34, 7.32, 1.3,
7.4, 1852.38, 4516.36, 6558.34, 1689.32, 1.3,
2.4, 52.38, 1289.36, 1289.34, 1646.32, 92.3,
2.4, 52.38, 9.36, 1610.34, 1486.32, 92.3,
2.4, 52.38, 2.36, 2.34, 1486.32, 92.3,
};

float mask[] = {
0, 0, 0.12, 0.14, 0, 0,
0, 0, 1, 0.9, 0, 0,
0.09, 0.19, 0.96, 0.75, 0.47, 0.19,
0, 0.13, 0.15, 0.39, 0.88, 0,
0, 0, 0.15, 0.39, 0.88, 0,
0, 0, 0.07, 0.19, 0, 0
};

int main(void) {/*
	bcc_timer_tick_init_period(10);
	bcc_timer_tick_init();*/
	break_simu(0,"Reset"/*,get_elapsed_time()*/);
	barycentre(window,mask);
	break_simu(1,"barycentre"/*,get_elapsed_time()*/);
	calculFluxBinaire(window,mask);
	break_simu(1,"calculFluxBinaire"/*,get_elapsed_time()*/);
	calculFluxPondere(window,mask);
	break_simu(1,"calculFluxPondere"/*,get_elapsed_time()*/);
	return EXIT_SUCCESS;
}

uint32_t break_simu(uint32_t break_id, const char * break_name/*, int32_t time*/){
	uint32_t static test_counter = 0;
	return test_counter++;
}

/*
int32_t get_elapsed_time() {
	static int32_t last_time = 0;
	int32_t elpased_time = 0;
	int32_t current_time = bcc_timer_get_us();
	elpased_time = current_time - last_time;
	last_time = current_time;
	return elpased_time;
}
*/
