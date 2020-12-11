/*  Test_task
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: tasks.c,v 1.7.2.1 2000/05/05 12:58:06 joel Exp $
 */


#include <rtems.h>
#include <assert.h>

/* functions */

rtems_task Init(
		rtems_task_argument argument
);

rtems_task task_1(rtems_task_argument unused);

rtems_task processingTask(rtems_task_argument unused);

rtems_task TelemetryManagerTask(rtems_task_argument unused);

void timer_1_entry(rtems_id timer_id, void* timer_input);
/* global variables */

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */ 

extern rtems_id   Task_id[ 4 ];         /* array of task ids */
extern rtems_name Task_name[ 4 ];       /* array of task names */


#define FLUX_LENGTH 10
typedef struct flux{
	uint32_t id_window ;
	uint32_t id_first_acquisition ; ///< valeur du compteur d'acquisition de measures[0]
	float measures[FLUX_LENGTH];
} flux ;

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_INIT
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
#define CONFIGURE_MAXIMUM_SEMAPHORES 3
#define CONFIGURE_MAXIMUM_TIMERS 5
#define CONFIGURE_MAXIMUM_TASKS             5
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 10
#define MESSAGE_QUEUE_COUNT 3
#define MESSAGE_QUEUE_SIZE 4

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (4 * RTEMS_MINIMUM_STACK_SIZE)

#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2
#define TASK3_PRIORITY 3
#define TASK_STACK_SIZE 10240

#include <rtems/confdefs.h>

/* If --drvmgr was enabled during the configuration of the RTEMS kernel */
#ifdef RTEMS_DRVMGR_STARTUP
#ifdef LEON3
/* Add Timer and UART Driver for this example */
#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
#endif
#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
#endif

#include <grlib/ambapp_bus.h>
/* OPTIONAL FOR GRLIB SYSTEMS WITH GPTIMER AS SYSTEM CLOCK TIMER */
struct drvmgr_key grlib_drv_res_gptimer0[] =
{
		/* If all timers should not be used (typically on an AMP system, or when timers
		 * are used customly in a project) one can limit to a range of timers.
		 * timerStart: start of range (0..6)
		 * timerCnt: Number of timers
		 */
#if 0
		{"timerStart", DRVMGR_KT_INT, {(unsigned int)SET_START}},
		{"timerCnt", DRVMGR_KT_INT, {(unsigned int)SET_NUMBER_FO_TIMERS}},
		/* Select Prescaler (Base frequency of all timers on a timer core) */
		{"prescaler", DRVMGR_KT_INT, {(unsigned int)SET_PRESCALER_HERE}},
		/* Select which timer should be used as the system clock (default is 0) */
		{"clockTimer", DRVMGR_KT_INT, {(unsigned int)TIMER_INDEX_USED_AS_CLOCK}},
#endif
		DRVMGR_KEY_EMPTY
};

/* Use GPTIMER core 4 (not present in most systems) as a high
 * resoulution timer */
struct drvmgr_key grlib_drv_res_gptimer4[] =
{
		{"prescaler", DRVMGR_KT_INT, {(unsigned int)4}},
		DRVMGR_KEY_EMPTY
};

struct drvmgr_bus_res grlib_drv_resources =
{
		.next = NULL,
		.resource = {
#if 0
				{DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 0, &grlib_drv_res_gptimer0[0]},
				{DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 1, NULL}, /* Do not use timers on this GPTIMER core */
				{DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 2, NULL}, /* Do not use timers on this GPTIMER core */
				{DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 3, NULL}, /* Do not use timers on this GPTIMER core */
				{DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 4, &grlib_drv_res_gptimer4[0]},
#endif
				DRVMGR_RES_EMPTY
		}
};
#endif

#include <drvmgr/drvmgr_confdefs.h>
#endif

/*
 *  Handy macros and static inline functions
 */

/*
 *  Macro to hide the ugliness of printing the time.
 */

#define print_time(_s1, _tb, _s2) \
		do { \
			iprintf( "%s%02"PRIu32":%02"PRIu32":%02"PRIu32"   %02"PRIu32"/%02"PRIu32"/%04"PRIu32"%s", \
					_s1, (_tb)->hour, (_tb)->minute, (_tb)->second, \
					(_tb)->month, (_tb)->day, (_tb)->year, _s2 ); \
					fflush(stdout); \
		} while ( 0 )

/*
 *  Macro to print an task name that is composed of ASCII characters.
 *
 */

#define put_name( _name, _crlf ) \
		do { \
			uint32_t c0, c1, c2, c3; \
			\
			c0 = ((_name) >> 24) & 0xff; \
			c1 = ((_name) >> 16) & 0xff; \
			c2 = ((_name) >> 8) & 0xff; \
			c3 = (_name) & 0xff; \
			putchar( (char)c0 ); \
			if ( c1 ) putchar( (char)c1 ); \
			if ( c2 ) putchar( (char)c2 ); \
			if ( c3 ) putchar( (char)c3 ); \
			if ( (_crlf) ) \
			putchar( '\n' ); \
		} while (0)

/*
 *  This allows us to view the "Test_task" instantiations as a set
 *  of numbered tasks by eliminating the number of application
 *  tasks created.
 *
 *  In reality, this is too complex for the purposes of this
 *  example.  It would have been easier to pass a task argument. :)
 *  But it shows how rtems_id's can sometimes be used.
 */

#define task_number( tid ) \
		( rtems_object_id_get_index( tid ) - \
				rtems_configuration_get_rtems_api_configuration()->number_of_initialization_tasks )


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "windows-producer.h"

#define windows_max_number 100

#include "buffer_circulaire-partagee.h"

Windows_producer wp;
float img_buffer[windows_max_number * 36];

flux tabFlux[windows_max_number];

buffer_circulaire_partagee buffer_circu;

uint8_t buffer[sizeof(flux) * 100];


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


/*
 *  Keep the names and IDs in global variables so another task can use them.
 */

rtems_id task_id_1;
rtems_id timer_id_1;
rtems_id message_queue_id_1;
rtems_id message_queue_id_2;
rtems_id task_id_2;
rtems_id task_id_3;
rtems_id semaphore_id_1;

rtems_id   Task_id[ 4 ];         /* array of task ids */
rtems_name Task_name[ 4 ];       /* array of task names */

rtems_task Init(rtems_task_argument argument)
{
	rtems_status_code status;
	rtems_time_of_day time;

	puts( "\n\n*** CLOCK TICK TEST ***" );

	time.year   = 1988;
	time.month  = 12;
	time.day    = 31;
	time.hour   = 9;
	time.minute = 0;
	time.second = 0;
	time.ticks  = 0;


	status = rtems_clock_set( &time );
	assert(status == RTEMS_SUCCESSFUL);

	Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
	Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
	Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );

	status = rtems_task_create(rtems_build_name('T','S','K', '1'),
			TASK1_PRIORITY, TASK_STACK_SIZE,
			RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_LOCAL | RTEMS_FLOATING_POINT, &task_id_1
	);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_create(rtems_build_name('T','S','K', '2'),
			TASK2_PRIORITY, TASK_STACK_SIZE,
			RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_LOCAL | RTEMS_FLOATING_POINT, &task_id_2
	);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_create(rtems_build_name('T','S','K', '3'),
			TASK3_PRIORITY, TASK_STACK_SIZE,
			RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_LOCAL | RTEMS_FLOATING_POINT, &task_id_3
	);
	assert(status == RTEMS_SUCCESSFUL);


	status = rtems_message_queue_create(rtems_build_name('M','S','Q','1'),
			MESSAGE_QUEUE_COUNT, MESSAGE_QUEUE_SIZE,
			RTEMS_LOCAL | RTEMS_PRIORITY, &message_queue_id_1);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_message_queue_create(rtems_build_name('M','S','Q','2'),
			MESSAGE_QUEUE_COUNT, MESSAGE_QUEUE_SIZE,
			RTEMS_LOCAL | RTEMS_PRIORITY, &message_queue_id_2);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_semaphore_create(rtems_build_name('S','E','M', '1'),1,
			RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE , 0,
			&semaphore_id_1);
	assert(status == RTEMS_SUCCESSFUL);


	init_buffer_circulaire_partagee(&buffer_circu,buffer, sizeof(flux), 100, semaphore_id_1);

	status = rtems_timer_create(rtems_build_name('T','I','M', '1'), &timer_id_1);
	assert(status == RTEMS_SUCCESSFUL);
	status = rtems_timer_fire_after(timer_id_1, 50, timer_1_entry, 0);
	assert(status == RTEMS_SUCCESSFUL);


	status = rtems_task_start(task_id_1, task_1, 1);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_start(task_id_2, processingTask, 1);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_start(task_id_3, TelemetryManagerTask, 1);
	assert(status == RTEMS_SUCCESSFUL);

	status = rtems_task_delete( RTEMS_SELF );
	assert(status == RTEMS_SUCCESSFUL);
}
#include <stdio.h>

uint32_t break_simu(const char * break_name, uint32_t nb_tick){
	uint32_t static test_counter = 0;
	return test_counter++;
}

uint32_t break_simu_float(const char * break_name, float a){
	uint32_t static test_counter = 0;
	return test_counter++;
}

int32_t get_elapsed_time() {
	static int32_t last_time = 0;
	int32_t elpased_time = 0;
	int32_t current_time = rtems_clock_get_ticks_since_boot();
	elpased_time = current_time - last_time;
	last_time = current_time;
	return elpased_time;
}

rtems_task task_1(rtems_task_argument unused){
	rtems_status_code status;
	uint32_t data = 0;
	static uint32_t counter = 0;
	size_t size;
	init(&wp, img_buffer, windows_max_number);
	while(1){
		produce_images(&wp);
		//rtems_task_wake_after(50);
		status = rtems_message_queue_receive(message_queue_id_1, &data, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		if(status == RTEMS_SUCCESSFUL){
			status = rtems_message_queue_send(message_queue_id_2, &counter, sizeof(uint32_t));
			assert(status == RTEMS_SUCCESSFUL);
			counter++;
		}
	}
}

rtems_task processingTask(rtems_task_argument unused){
	rtems_status_code status;
	uint32_t count = 0;
	size_t size;
	for(int i = 0; i<windows_max_number; i++){
		tabFlux[i].id_first_acquisition = 0;
	}
	while(1){
		status = rtems_message_queue_receive(message_queue_id_2, &count, &size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		//		status = RTEMS_SUCCESSFUL;
		if(status == RTEMS_SUCCESSFUL){
			for(int i = 0; i< windows_max_number; i++){
				tabFlux[i].id_window = i;
				if(count - tabFlux[i].id_first_acquisition >= 10) tabFlux[i].id_first_acquisition = count;
				tabFlux[i].measures[count-tabFlux[i].id_first_acquisition] = calculFluxPondere(&img_buffer[i*36], get_mask(&wp, i));
				if(count - tabFlux[i].id_first_acquisition >= 9){
					push_partagee(&buffer_circu,(uint8_t *) &tabFlux[i], sizeof(flux));
				}
			}
		}
	}
}

/**
 * Simule l'émission d'une séquence de mesures de photométrie.
 * @param f structure enregistrant la séquence
 * @param time moment de l'émission, exprimé en ticks depuis le démarrage du programme .
 */
void send_flux(uint32_t time, flux f){
	static int cpt = 0 ;
	cpt++ ;
}


rtems_task TelemetryManagerTask(rtems_task_argument unused){
	const int intervalleRegu = 10;
	const int nbFluxMax = 30;
	int debutSlot;
	int nbFlux;
	uint8_t destination[sizeof(flux)];
	while(1){
		//while(pop_partagee(&buffer_circu, destination, sizeof(flux)) < 0){}
		//debutSlot = rtems_clock_get_ticks_since_boot();
		//nbFlux = 1;
		//send_flux(rtems_clock_get_ticks_since_boot(),*(flux*)destination);

		//while(nbFlux < nbFluxMax && debutSlot + intervalleRegu > rtems_clock_get_ticks_since_boot()){
			if (pop_partagee(&buffer_circu, destination, sizeof(flux)) > 0){
				send_flux(rtems_clock_get_ticks_since_boot(),*(flux*)destination);
		//		nbFlux++;
			}
		//}
		//if(debutSlot + intervalleRegu > rtems_clock_get_ticks_since_boot()){
		//	rtems_task_wake_after(debutSlot + intervalleRegu - rtems_clock_get_ticks_since_boot());
		//}
	}
}



void timer_1_entry(rtems_id timer_id, void* timer_input) {
	static uint32_t data = 0;
	rtems_status_code status;
	data++;
	status = rtems_message_queue_send(message_queue_id_1, &data, sizeof(uint32_t));
	assert(status == RTEMS_SUCCESSFUL);
	status = rtems_timer_fire_after(timer_id_1, 50, timer_1_entry, 0); // 10 ticks = 100 ms
	assert(status == RTEMS_SUCCESSFUL);
}




rtems_task Test_task(
		rtems_task_argument unused
)
{
	rtems_id          tid;
	rtems_time_of_day time;
	uint32_t  task_index;
	rtems_status_code status;

	status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
	assert(status == RTEMS_SUCCESSFUL);
	task_index = task_number( tid );
	for ( ; ; ) {
		status = rtems_clock_get_tod( &time );
		assert(status == RTEMS_SUCCESSFUL);
		if ( time.second >= 3335 ) {
			puts( "*** END OF CLOCK TICK TEST ***" );
			exit( 0 );
		}
		put_name( Task_name[ task_index ], FALSE );
		print_time( " - rtems_clock_get - ", &time, "\n" );
		status = rtems_task_wake_after( task_index * 5 * rtems_clock_get_ticks_per_second() );
		assert(status == RTEMS_SUCCESSFUL);
	}
}
