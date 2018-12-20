/**
*	Author: Francisco Pinto Santos
*	Year: 2018
*	thx to Gyermo from usal: http://avellano.usal.es/~ssooi/pract119.htm
*/

#include "header.h"

int fd;
int * villianPids;

void charley(void);
void bosley(void);
void angel(void);
void villian(void);
void villianHandler(int ss);
void charleyHandler(int ss);
void bosleyHandler(int ss);
void angelHandler(int ss);
void initializeMappedFile(void);
void closeMappedFile(void);
void logStdin(char * str);

void sigtermHandler(int ss){

}


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
	EXIT_ON_FAILURE(sigdelset(&allSignalsSet,SIGNAL_TERMINATE));
	REDEFINE_SIGNAL(SIGNAL_TERMINATE,sigtermHandler);
	EXIT_ON_FAILURE(sigprocmask(SIG_SETMASK, &allSignalsSet, NULL));

	//initialize map 
	initializeMappedFile();

	//switch the argv name
	if(!strcmp(argv[0],CHARLIE_ARG)){
		memset(villianPids,0,MAP_SIZE);
		charley();
	}else if(!strcmp(argv[0],BOSLEY_ARG)){
		bosley();
	}else if(!strcmp(argv[0],MALO_ARG)){
		villian();
	}else if(!strcmp(argv[0],SABRINA_ARG) || strcmp(argv[0],KELLY_ARG) || strcmp(argv[0],JILL_ARG)){
		angel();
	}else{
		EXIT_ON_FAILURE(-1);
	}
}

void charley(void){
	int status;
	pid_t pbosley, pvillian, precived;
	sigset_t angelsCreated;


int i;

	//fill handlers
	EXIT_ON_FAILURE(sigfillset(&angelsCreated));
	EXIT_ON_FAILURE(sigdelset(&angelsCreated, SIGNAL_ANGELS_CREATED));
	//redefine all possible signals 
	REDEFINE_SIGNAL(SIGNAL_ANGELS_CREATED,charleyHandler);
	//create bosley process
	pbosley = fork();
	if(pbosley == 0){
		REDEFINE_EXECUTABLE_NAME(CHARLIE_ARG,BOSLEY_ARG);
	}
	//wait for the angels creation
	sigsuspend(&angelsCreated);
	//create villian
	pvillian = fork();
	if(pvillian == 0){
		REDEFINE_EXECUTABLE_NAME(CHARLIE_ARG,MALO_ARG);
	}
	//advise bosley villian has been created
	kill(pbosley,SIGNAL_VILLIAN_CREATED);
	//wait for final result
	while(TRUE){
		status = 0;
		precived = wait(&status);
		
		if(precived == pbosley){
			switch(WEXITSTATUS(status)){
				case EXIT_BOSLEY_SABRINA_SUCESS: logStdin("[BOSLEY]sabrina hit thte target"); break;
				case EXIT_BOSLEY_JILL_SUCESS:    logStdin("[BOSLEY]jill hit thte target"); break;
				case EXIT_BOSLEY_KELLY_SUCESS:   logStdin("[BOSLEY]kelly hit thte target"); break;
				case EXIT_BOSLEY_ANGELS_FAILED:  logStdin("[BOSLEY]angels failed"); break;
			}

			for(i=0; i<20;i++){
				fprintf(stderr,"\n%d",villianPids[i]);
			}

			closeMappedFile();
			//kill the rest of process
			kill(0,SIGNAL_TERMINATE);
			exit(EXIT_SUCCESS);
		}
	}
}
void bosley(void){
	int status, i;
	pid_t psabrina,pjill,pkelly,precived;
	sigset_t villiancreated;

	//fill masks
	EXIT_ON_FAILURE(sigfillset(&villiancreated));
	EXIT_ON_FAILURE(sigdelset(&villiancreated, SIGNAL_VILLIAN_CREATED));
	//redefine all possible signals 
	REDEFINE_SIGNAL(SIGNAL_VILLIAN_CREATED,bosleyHandler);

	//create angels
	psabrina = fork();
	if(psabrina == 0){
		REDEFINE_EXECUTABLE_NAME(CHARLIE_ARG,SABRINA_ARG);
	}
	pjill = fork();
	if(pjill == 0){
		REDEFINE_EXECUTABLE_NAME(CHARLIE_ARG,JILL_ARG);
	}
	pkelly = fork();
	if(pkelly == 0){
		REDEFINE_EXECUTABLE_NAME(CHARLIE_ARG,KELLY_ARG);
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
	for(i=0; i<N_ANGELS; i++){
		status = 0;
		precived = wait(&status);

		if(EXIT_ANGEL_HIT_TARGET == WEXITSTATUS(status)){
			if(precived == psabrina)
				exit(EXIT_BOSLEY_SABRINA_SUCESS);
			else if(precived == pjill)
				exit(EXIT_BOSLEY_JILL_SUCESS);
			else
				exit(EXIT_BOSLEY_KELLY_SUCESS);
		}
	}
	//exit if angels has failed
	exit(EXIT_BOSLEY_ANGELS_FAILED);
}

void angel(){
	int i, target, timeToStartShotting, killResult;
	sigset_t startShot;

	//fill masks
	EXIT_ON_FAILURE(sigfillset(&startShot));
	EXIT_ON_FAILURE(sigdelset(&startShot, SIGNAL_START_SHOT));
	//redefine all possible signals 
	REDEFINE_SIGNAL(SIGNAL_START_SHOT,angelHandler);

	timeToStartShotting = GET_NEW_SLEEP_TIME;
	sigsuspend(&startShot);
	sleep(timeToStartShotting);

	for(i=0; i<N_SHOT_TRIES; i++){
		target = GET_NEW_TARGET;
		fprintf(stderr, "sad%d\n",villianPids[0]);
		if(0!=villianPids[0]){
			killResult = kill(SIGNAL_SHOT,villianPids[target]);
			if(killResult != -1){
				logStdin("[ANGEL] acerte en uno");
				exit(EXIT_ANGEL_HIT_TARGET);
			}else
				logStdin("[ANGEL] fallie wey");
		}
	}

	//exit if angel failed
	exit(EXIT_ANGEL_FAILED);
}

void villian(void){
	pid_t newChild;
	int reencarnationsLeft, timeToReincarnation, writePosition;
	sigset_t waitForShotOrReencarnationMask;

	//fill masks
	EXIT_ON_FAILURE(sigfillset(&waitForShotOrReencarnationMask));
	EXIT_ON_FAILURE(sigdelset(&waitForShotOrReencarnationMask, SIGNAL_SHOT));
	EXIT_ON_FAILURE(sigdelset(&waitForShotOrReencarnationMask, SIGNAL_REENCARNATION));
	//redefine all possible signals 
	REDEFINE_SIGNAL(SIGNAL_SHOT,villianHandler);
	REDEFINE_SIGNAL(SIGNAL_REENCARNATION,villianHandler);

	reencarnationsLeft = N_REENCARNATIONS;

	while(TRUE){
		//write pid in file
		writePosition = -1;
		while(writePosition<0 || villianPids[writePosition]!= 0){
			writePosition = GET_WRITE_POSITION;
		}
		villianPids[0] = getpid();
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
				exit(EXIT_VILLIAN_REENCARNATED);
		}else{
			logStdin("[VILLIAN] no reencarnations left");
			exit(EXIT_VILLIAN_NO_REENCARNATIONS_LEFT);
		}
	}
}

void villianHandler(int ss){
	//here we only print messages
	if( ss == SIGNAL_SHOT){
		logStdin("asesinado");
		exit(EXIT_VILLIAN_MURDERED);
	}
}
void charleyHandler(int ss){
}
void angelHandler(int ss){
}
void bosleyHandler(int ss){
}

//obtain descriptor an do the file mapping
void initializeMappedFile(){
	int foo = 0;

	EXIT_ON_FAILURE(fd= open(PIDS_FILE, O_CREAT | O_RDWR, 0600));

	//write to avoid an error
	write(fd,&foo,sizeof(int));

	villianPids = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == villianPids)
		EXIT_ON_FAILURE(-1);
}
void closeMappedFile(void){
	EXIT_ON_FAILURE(munmap((void*)villianPids, MAP_SIZE));
	EXIT_ON_FAILURE(close(fd));
}

void logStdin(char * str){
	char tag[100];
	sprintf(tag, "\n[%d]%s",getpid(),str);
	write(STDERR_FILENO,tag,strlen(tag));
}
