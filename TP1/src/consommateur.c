#include <stdint.h>

/**
* Considérant le premier tableau comme contenant des valeurs de rayon, la fonction
itérera le nombre de fois défini en entrée et enregistrera dans le second le résultat du calcul du
périmètre
* @param rayon_array tableau contenant les rayons
* @param perimetre_array tableau où le périmètre est enregistré
* @param size nombre de valeur à inscrire dans \a perimetre_array
*/
void rayon2perim(float rayon_array[], float perimetre_array[], uint32_t size){
	test();
	float pi = 103993.0/33102;
	test();
	for(int i = 0; i < size; i++){
		perimetre_array[i] = 2*rayon_array[i]*pi;
	}
}
