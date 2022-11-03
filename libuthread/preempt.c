#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"


bool pr; 
struct itimerval Globe_timer;
struct sigaction Globe_action;	
/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

void preempt_disable(void)
{
	/* TODO Phase 4
	
	allow the timer to run
	1. create a segset_t temp
	2. check if preempt == 1
	3. call seg_emptyset, seg_addsig(temp), seg_propmask(block/unblock, temp,NULL )
	*/
	
	sigset_t temp;
	if(pr){				// checking if preempt is enabled
	sigemptyset(&temp); // Start with an empty set of signals.
  	sigaddset(&temp, SIGALRM); // Add the SIGALRM signal to the set.
	sigprocmask(SIG_BLOCK, &temp, NULL);	// examines the signals
	}
}

void preempt_enable(void)
{
	/* TODO Phase 4 
	allow the timer to run
	1. create a segset_t temp
	2. check if preempt == 1
	3. call seg_emptyset, seg_addsig(temp), segvt_alrm, seg_propmask(block/unblock, temp,NULL )	
	*/
 	
	sigset_t temp; 
	if(pr){ // checking if preempt is enabled
	sigemptyset(&temp); // Start with an empty set of signals.
  	sigaddset(&temp, SIGALRM); // Add the SIGALRM signal to the set.
	sigprocmask(SIG_UNBLOCK, &temp, NULL); // examines the signals
	}
}


void handler(int temp){
	uthread_yield();
}
/*
The following code is inspired by this resource:
https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm
*/
void preempt_start(bool preempt)
{
	pr = preempt;
	if(preempt){
	// creating a signal handler struct
	struct sigaction action;
	// sending the segnals to specific function
	action.sa_handler = handler;
	sigemptyset (&action.sa_mask);
	// setting up the actions
	sigaction(SIGVTALRM, &action, &Globe_action);  

	struct itimerval timer;
	timer.it_value.tv_usec = 10000000/HZ;
	timer.it_value.tv_sec = 0;
	timer.it_interval.tv_usec = 10000000/HZ;
	timer.it_interval.tv_sec = 0;
	timer.it_interval = timer.it_value; 
	setitimer (ITIMER_REAL, &timer, &Globe_timer);

	}

}

void preempt_stop(void)
{
	sigaction(SIGVTALRM, &Globe_action, NULL); 
	setitimer (ITIMER_REAL, &Globe_timer, NULL);

}
