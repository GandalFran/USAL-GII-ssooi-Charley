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
#define CHARLIE_ARG "charlie"
#define BOSLEY_ARG "bosley"
#define MALO_ARG "malo"
#define SABRINA_ARG "sabrina"
#define KELLY_ARG "kelly"
#define JILL_ARG "jill"

#define NORMAL_ARG "normal"
#define FAST_ARG "fast"
#define USAGE_ERROR "./charlie [fast | normal]"

//constants
#define N_REENCARNATIONS 20
#define N_SHOT_TRIES 3
#define MAP_SIZE (sizeof(int)*20)
#define PIDS_FILE "pids.bin"

//define signal names 
#define SIGNAL_SHOT SIGTERM
#define SIGNAL_START_SHOT SIGUSR1
#define SIGNAL_REENCARNATION SIGALRM
#define SIGNAL_VILLIAN_CREATED SIGUSR2
#define SIGNAL_ANGELS_CREATED SIGUSR1

//random numbers
#define GET_RANDOM(min,max) ( (min) + ( rand() * ((max)-(min)) ) )
#define GET_NEW_SLEEP_TIME ( GET_RANDOM(6,12))
#define GET_TIME_TO_REENCARNATE ( GET_RANDOM(1,3))
#define GET_NEW_TARGET ( GET_RANDOM(0,N_REENCARNATIONS-1))



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
    	logStdin("changed");									\
    	char nameTag[80], nameTag2[80];							\
    	sprintf(nameTag,"%s/%s",EXECUTABLE_PATH,actualName);	\
    	sprintf(nameTag2,"%s/%s",EXECUTABLE_PATH,newName);		\
 		EXIT_ON_FAILURE(execl(nameTag, nameTag2, NULL));		\
	}while(0)	

#endif
