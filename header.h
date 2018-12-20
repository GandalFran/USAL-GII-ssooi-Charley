#ifndef __HEADER_H
#define __HEADER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef unsigned short bool;
typedef enum {NONE_MODE, NORMAL_MODE, FAST_MODE} Mode;

#define TRUE    1
#define FALSE   0


#define EXECUTABLE_PATH "."

//args
#define CHARLIE_ARG "./charlie"
#define BOSLEY_ARG "./bosley"
#define MALO_ARG "./malo"
#define SABRINA_ARG "./sabrina"
#define KELLY_ARG "./kelly"
#define JILL_ARG "./jill"

#define NORMAL_ARG "normal"
#define FAST_ARG "fast"
#define USAGE_ERROR "./charlie [fast | normal]"

//constants
#define N_REENCARNATIONS 20
#define N_SHOT_TRIES 3
#define N_ANGELS 3
#define MAP_SIZE (sizeof(int)*N_REENCARNATIONS)
#define PIDS_FILE "pids.bin"

//define signal names 
#define SIGNAL_SHOT SIGTERM
#define SIGNAL_START_SHOT SIGUSR1
#define SIGNAL_REENCARNATION SIGALRM
#define SIGNAL_VILLIAN_CREATED SIGUSR1
#define SIGNAL_ANGELS_CREATED SIGUSR2
#define SIGNAL_TERMINATE SIGINT

//random numbers
#define GET_RANDOM(min,max) ( (min)+(int)(rand()/(1.0+RAND_MAX)*((max)-(min)+1)) )
#define GET_NEW_SLEEP_TIME ( GET_RANDOM(6,12))
#define GET_TIME_TO_REENCARNATE ( GET_RANDOM(1,3))
#define GET_NEW_TARGET ( GET_RANDOM(0,N_REENCARNATIONS-1))
#define GET_WRITE_POSITION ( GET_RANDOM(0,N_REENCARNATIONS-1))

//exit codes
#define EXIT_BOSLEY_SABRINA_SUCESS 0
#define EXIT_BOSLEY_JILL_SUCESS 1
#define EXIT_BOSLEY_KELLY_SUCESS 2
#define EXIT_BOSLEY_ANGELS_FAILED 3

#define EXIT_ANGEL_HIT_TARGET 0
#define EXIT_ANGEL_FAILED 1

#define EXIT_VILLIAN_REENCARNATED 0
#define EXIT_VILLIAN_NO_REENCARNATIONS_LEFT 1
#define EXIT_VILLIAN_MURDERED 2

//Log errors
#define LOG(returnValue)                                                   		 	\
    do{                                                                             \
        if((returnValue) == -1){                                                    \
            char errorTag[80];                                                      \
            sprintf(errorTag, "\n[%s:%d:%s] ", __FILE__, __LINE__, __FUNCTION__);   \
            perror(errorTag);                                                       \
        }                                                                           \
    }while(0)

#define EXIT_ON_FAILURE(returnValue)            \
    do{                                         \
        if((returnValue) == -1){                \
            LOG(-1);                    		\
        }                                       \
    }while(0)

#define EXIT_IF_NULL(returnValue)               \
    do{                                         \
        if((returnValue) == NULL){              \
            LOG(-1);                    		\
        }                                       \
    }while(0)

//process and signals 

#define CREATE_PROCESS(value)    EXIT_ON_FAILURE(value = fork())

#define IF_CHILD(value) if(value == 0)

#define KILL_PROCESS(pid,signal)        \
    do{                                 \
        LOG(kill(pid,signal));          \
        LOG(waitpid(pid,NULL,0));       \
    }while(0)

#define REDEFINE_SIGNAL(signal,funcion)                         \
    do{                                                         \
        struct sigaction sigactionSs;                           \
        EXIT_ON_FAILURE(sigfillset(&sigactionSs.sa_mask));      \
        sigactionSs.sa_handler=funcion;                         \
        sigactionSs.sa_flags=0;                                 \
        EXIT_ON_FAILURE(sigaction(signal,&sigactionSs,NULL));   \
    }while(0)

//others 
#define REDEFINE_EXECUTABLE_NAME(actualName,newName)			\
    do{															\
 		EXIT_ON_FAILURE(execl(actualName, newName, NULL));		\
	}while(0)	

#endif
