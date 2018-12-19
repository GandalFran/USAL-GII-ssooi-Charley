/**
*	Author: Francisco Pinto Santos
*	Year: 2018
*	thx to Gyermo from usal: http://avellano.usal.es/~ssooi/pract119.htm
*/

#include "header.h"

int fd;
int * childPids;

void charley(void);
void bosley(void);
int angel(void);
void villian(void);
void villianHandler(int ss);
void initializeMappedFile(void);
void closeMappedFile(void);
void logStdin(char * str);

int main(int argc, char *argv[]){
	Mode mode;
	sigset_t allSignalsSet;

	if( argc > 2 || ( argc==2 && (!strcmp(argv[1],NORMAL_ARG) && !strcmp(argv[1],FAST_ARG)) ) ){
		fprintf(stderr, "%s\n",USAGE_ERROR);
		exit(EXIT_FAILURE);
	}else if(argc == 2){
		mode = (!strcmp(argv[1],NORMAL_ARG)) ? NORMAL_MODE : FAST_MODE;
	}else{
		mode = NONE_MODE;
	}
	
	//block all signals
	EXIT_ON_FAILURE(sigfillset(&allSignalsSet));
	EXIT_ON_FAILURE(sigprocmask(SIG_SETMASK, &allSignalsSet, NULL));

	//redefine sigchild to be ignored and let the child process die in peace
	REDEFINE_SIGNAL(SIGCHLD,SIG_IGN);

	//go to charly
	charley();
}

void charley(void){
	pid_t pbosley, pvillian, precived;
	sigset_t angelsCreated;

	//fill handlers
	EXIT_ON_FAILURE(sigfillset(&angelsCreated));
	EXIT_ON_FAILURE(sigdelset(&angelsCreated, SIGNAL_ANGELS_CREATED));
	//create bosley process
	pbosley = fork();
	if(pbosley == 0){
		REDEFINE_EXECUTABLE_NAME
		bosley();
	}
	//wait for the angels creation
	sigsuspend(&angelsCreated);
	//create villian
	pvillian = fork();
	if(pvillian == 0){
		REDEFINE_EXECUTABLE_NAME
		villian();
	}
	//advise bosley villian has been created
	kill(pbosley,SIGNAL_VILLIAN_CREATED);
	//wait for final result
	while(TRUE){
		precived = waitpid(-1, NULL, 0);
		if(precived == pvillian){
			logStdin("villian died");
			exit(EXIT_SUCCESS);
		}
	}
}

void bosley(void){
	pid_t psabrina,pjill,pkelly,precived;
	sigset_t villiancreated;

	REDEFINE_EXECUTABLE_NAME

	EXIT_ON_FAILURE(sigfillset(&villiancreated));
	EXIT_ON_FAILURE(sigdelset(&villiancreated, SIGNAL_VILLIAN_CREATED));

	//create angels
	psabrina = fork();
	if(psabrina == 0){
		REDEFINE_EXECUTABLE_NAME
		angel();
	}
	pjill = fork();
	if(pjill == 0){
		REDEFINE_EXECUTABLE_NAME
		angel();
	}
	pkelly = fork();
	if(pkelly == 0){
		REDEFINE_EXECUTABLE_NAME
		angel();
	}
	//advise father the angels has been created
	kill(getppid(),SIGNAL_ANGELS_CREATED); 
	//wait for villian created 
	sigsuspend(&villiancreated);
	//advise start shot 
	kill(psabrina,SIGNAL_START_SHOT); 
	kill(pjill,SIGNAL_START_SHOT); 
	kill(pkelly,SIGNAL_START_SHOT); 
	//wait for final event
	while(TRUE){
		precived = waitpid(-1, NULL, 0);
		if(precived == psabrina){
			logStdin("sabrina died");
			exit(EXIT_SUCCESS);
		}else{
			logStdin("other died");
		}
	}
}

int angel(){
	int i, target, timeToStartShotting;

	timeToStartShotting = GET_NEW_SLEEP_TIME;

	sleep(timeToStartShotting);

	for(i=0; i<N_SHOT_TRIES; i++){
		target = GET_NEW_TARGET;
		if(0!=childPids[target]){
			kill(SIGNAL_SHOT,childPids[target]);
			logStdin("acerte en uno, ojala sea el verdadero");
			return 1;
		}
	}
	return 0;

}

void villian(void){
	pid_t newChild;
	int reencarnationsLeft, timeToReincarnation;
	sigset_t waitForShotOrReencarnationMask;

	EXIT_ON_FAILURE(sigfillset(&waitForShotOrReencarnationMask));
	EXIT_ON_FAILURE(sigdelset(&waitForShotOrReencarnationMask, SIGNAL_SHOT));
	EXIT_ON_FAILURE(sigdelset(&waitForShotOrReencarnationMask, SIGNAL_REENCARNATION));

	REDEFINE_SIGNAL(SIGNAL_SHOT,villianHandler);
	REDEFINE_SIGNAL(SIGNAL_REENCARNATION,villianHandler);

	reencarnationsLeft = N_REENCARNATIONS;

	while(TRUE){
		//write pid in file
		childPids[N_REENCARNATIONS-reencarnationsLeft] = getpid();
		//reduce reencarnations and get time to reencarnate
		reencarnationsLeft -= 1;
		timeToReincarnation = GET_TIME_TO_REENCARNATE;
		//activate the reencarnation timeout and wait for being shoted
		alarm(timeToReincarnation);
		sigsuspend(&waitForShotOrReencarnationMask);
		//here is where we come back when we have entered in handler
		if(reencarnationsLeft > 0){
			newChild = fork();
			if(newChild != 0)
				exit(EXIT_SUCCESS);
		}
	}
}

void villianHandler(int ss){
	//here we only print messages
	if(ss == SIGNAL_REENCARNATION){
		logStdin("reencarnado");
	}else if( ss == SIGNAL_SHOT){
		logStdin("asesinado");
	}
}

//obtain descriptor an do the file mapping
void initializeMappedFile(){
	int foo = 0;

	EXIT_ON_FAILURE(fd= open(PIDS_FILE, O_CREAT | O_RDWR, 0600));

	//write to avoid an error
	write(fd,&foo,sizeof(int));

	childPids = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == childPids)
		EXIT_ON_FAILURE(-1);
	
	//set all to zero
	memset(childPids,0,MAP_SIZE);
}
void closeMappedFile(void){
	EXIT_ON_FAILURE(munmap((void*)childPids, MAP_SIZE));
	EXIT_ON_FAILURE(close(fd));
}

void logStdin(char * str){
	write(1,str,strlen(str));
}


