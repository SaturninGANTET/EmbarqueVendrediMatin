#include "buffer_circulaire-partagee.h"

void init_buffer_circulaire_partagee(buffer_circulaire_partagee* b, uint8_t* buffer, uint16_t taille_donnee, uint32_t nombre_donnees, rtems_id semaphore_id) {
	init_buffer_circulaire( &(b->buffer_circu), buffer, taille_donnee, nombre_donnees);
	b->semaphore_id = semaphore_id;
}


int push_partagee(buffer_circulaire_partagee* fifo, uint8_t* source, uint16_t taille) {
	int res;
	rtems_status_code status;
	status = rtems_semaphore_obtain(fifo->semaphore_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(status == RTEMS_SUCCESSFUL);
	res = push(&(fifo->buffer_circu), source,taille);
	status = rtems_semaphore_release(fifo->semaphore_id);
	assert(status == RTEMS_SUCCESSFUL);
	return res;

}


int pop_partagee(buffer_circulaire_partagee* fifo, uint8_t* destination, uint16_t taille_max) {
	int res;
	rtems_status_code status;
	status = rtems_semaphore_obtain(fifo->semaphore_id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	assert(status == RTEMS_SUCCESSFUL);
	res = pop(&(fifo->buffer_circu),destination,taille_max);
	status = rtems_semaphore_release(fifo->semaphore_id);
	assert(status == RTEMS_SUCCESSFUL);
	return res;
}
