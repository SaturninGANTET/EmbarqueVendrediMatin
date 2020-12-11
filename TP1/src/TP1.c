/*
 ============================================================================
 Name        : TP1.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
uint32_t tableau[20] __attribute__ ((section (".bss_tableau")));
float tableau2[20] __attribute__ ((section(".bss_tableau")));



int main(void) {
	fibonacci(tableau, 20, 15);
	rayon2perim((float*)tableau, tableau2, 20);
	test();
	return EXIT_SUCCESS;
}
