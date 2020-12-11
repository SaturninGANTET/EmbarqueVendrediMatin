#include <stdint.h>

/**
* Remplis un tableau avec la suite de Fibonnacci.
* @param output_array tableau où la suite est enregistré
* @param size nombre de valeur à inscrire dans \a output_array
* @param min_value valeur jusqu'à laquelle les nombres ne sont pas enregistrés
*/
void fibonacci(uint32_t output_array[], uint32_t size, uint32_t min_value){
	int val1= 1;
	int val2= 1;
	int dummy;

	while(val2 < min_value){
		dummy = val1;
		val1 = val2;
		val2 += dummy;
	}

	for(int i = 0; i < size; i++){
		output_array[i] = val2;
		dummy = val1;
		val1 = val2;
		val2 += dummy;
	}
}

void test(){
	return;
}


void display_factorial( uint32_t * array,
uint32_t n)
{
uint32_t i = 0;
for ( i = 0; i < n; i++)
{
printf("fact(%d) = %d\n", i, array[i]);
}
}
