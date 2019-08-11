#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//  osevents.c, written by Chris.McDonald@uwa.edu.au, September 2016
//  compile with:  cc -std=c99 -Wall -Werror -pedantic -o osevents osevents.c

#define MAXPID		50
#define MAXIOS		100

#define UNKNOWN		(-1)
#define HAS_EXITED	(-1)

#define PID_IDLE	(-1)

#define IO_READ		0
#define IO_WRITE	1
#define IO_VERIFY	2

//  THE OS CHARACTERISTICS READ FROM THE SCENARIO FILE
int	timeQuantum		= 1;
int	diskRPM			= 1;
int	diskSectors		= 1;

//  THE THREE STATISTICS WE NEED TO ACCUMULATE
int	totalTurnaroundTime	= 0;
int	totalBlockedReadTime	= 0;
int	totalBlockedWriteTime	= 0;

//  -----------------------------------------------------------------------

//  WE MAINTAIN 3 SIMILAR 2-D ARRAYS -
//	ONE TO HOLD EACH PROCESS'S TIMES OF ITS EVENTS,
//	ONE HOLD THE REQUIRED SECTOR OF ITS i/o EVENTS, and
//	ONE TO RECORD IF THE i/o REQUEST WAS A read OR A write.

//  ARRAYS ARE ALL USED THE SAME WAY  (REMEMBER THAT THERE MAY BE NO i/o):
//
//	array[pid][0]	- absolute time (or sector) of process's admit event
//	array[pid][1]	- relative time (or sector) of process's 1st i/o event
//	array[pid][2]	- relative time (or sector) of process's 2nd i/o event
//	.....
//	array[pid][x]	- relative time (or sector) of process's exit event
//	array[pid][x+1] - holds constant HAS_EXITED (hence, no more events)
//
int	events [MAXPID][MAXIOS+3];
int	sectors[MAXPID][MAXIOS+3];
int	reason [MAXPID][MAXIOS+3];

//  -----------------------------------------------------------------------

//  COPY CHARACTERS FROM CURRENT POSITION IN LINE TO FORM THE NEXT WORD
int copy_word(char word[], char line[], int c)
{
    int w=0;

    while(line[c] != '\0' && !isspace(line[c])) {
	word[w] = line[c];
	++w;
	++c;
    }
    word[w]	= '\0';		// don't forget the null-byte!

    while(isspace(line[c])) {	// skip any trailing spaces
	++c;
    }
    return c;			// return where we're up to in 'line'
}

//  READ THE SCENARIO FILE (NO NEED TO CHECK FOR ANY ERRORS)
int read_scenario_file( char filename[] )
{
//  ATTEMPT TO OPEN NAMED FILE, OR USE stdin
    FILE *fp;

    if(strcmp(filename, "-") == 0)
	fp	= stdin;
    else {
	fp = fopen(filename, "r");
	if(fp == NULL) {
	    perror(filename);
	    exit(EXIT_FAILURE);
	}
    }

    int		nprocs	= 0;
    int		nevents[MAXPID];
    char	line[BUFSIZ];

//  READ EVENTS UNTIL END-OF-FILE REACHED
    while(fgets(line, sizeof line, fp) != NULL) {
	char	word[BUFSIZ];

	int c	= copy_word(word, line, 0);		// field-1

//  DETECT THE 3 SYSTEM CHARACTERISTICS
	if(strcmp(word, "timequantum") == 0) {
	    c	= copy_word(word, line, c);		// field-2
	    timeQuantum = atoi(word);
	}
	else if(strcmp(word, "diskrpm") == 0) {
	    c	= copy_word(word, line, c);
	    diskRPM	= atoi(word);
	}
	else if(strcmp(word, "disksectors") == 0) {
	    c	= copy_word(word, line, c);
	    diskSectors = atoi(word);
	}

//  OTHERWISE ASSUME THAT IT'S AN EVENT LINE
	else if(isdigit(word[0])) {			// field-1
	    int usecs	= atoi(word);

	    c		= copy_word(word, line, c);	// field-2
	    int PID	= atoi(word);

	    c		= copy_word(word, line, c);	// field-3

	    if(strcmp(word, "admit") == 0) {
		events [PID][0]		= usecs;	// time is absolute
		nevents[PID]		= 1;
		++nprocs;
	    }
	    else if(strcmp(word, "read") == 0) {
		int next		= nevents[PID];

		events [PID][next]	= usecs;	// time is relative
		c			= copy_word(word, line, c);
		sectors[PID][next]	= atoi(word);
		reason [PID][next]	= IO_READ;
		++nevents[PID];
	    }
	    else if(strcmp(word, "write") == 0) {
		int next		= nevents[PID];

		events [PID][next]	= usecs;	// time is relative
		c			= copy_word(word, line, c);
		sectors[PID][next]	= atoi(word);
		reason [PID][next]	= IO_WRITE;
		++nevents[PID];
	    }
	    else if(strcmp(word, "exit") == 0) {
		int next		= nevents[PID];

		events[PID][next]	= usecs;	// time is relative
		events[PID][next+1]	= HAS_EXITED;
	    }
	}
    }

//  END OF SCENARIO FILE, CLOSE FILE, RETURN NUMBER OF PROCESSES FOUND
    fclose(fp);
    return nprocs;
}

//  -----------------------------------------------------------------------

//  THE READY QUEUE IS IMPLEMENTED WITH AN ARRAY, WHICH CAN BE AT MOST MAXPID
//  ELEMENTS LONG (IF ALL PROCESSES WERE 'READY'). WE ALWAYS ADD NEWLY READY
//  EVENTS TO THE END/TAIL OF THE QUEUE, AND REMOVE THE EVENTS FROM THE HEAD
//  WHEN IT IS AVAILABLE TO USE THE CPU (BECOMES RUNNING)
int	readyQueue[MAXPID];
int	nReady		= 0;

void enqueueReady(int PID)
{
    readyQueue[nReady] = PID;		// add to the tail of the queue
    ++nReady;
}

int dequeueReady(void)
{
    int PID    = readyQueue[0];		// remove from the head of queue

    --nReady;
    for(int r=0 ; r<nReady ; ++r) {	// 'squash up' remaining processes
	readyQueue[r]	= readyQueue[r+1];
    }
    return PID;
}

//  -----------------------------------------------------------------------

//  THE BLOCKED QUEUE IS IMPLEMENTED WITH 5 ARRAYS, EACH AT MOST MAXPID
//  ELEMENTS LONG (IF ALL PROCESSES WERE 'BLOCKED').  FOR EACH BLOCKED
//  PROCESS WE REMEMBER ITS PID, THE SECTOR IT REQUIRES, AND THE TIME IT
//  BECAME BLOCKED.  NEWLY-BLOCKED PROCESSES ARE ALWAYS APPENDED TO THE
//  END/TAIL OF THE ARRAYS, AND THUS THE ARRAYS ARE 'SORTED' IN TIME-ORDER,
//  BUT ANY PROCESSES MAY (ALL) BE REMOVED FROM THE QUEUE (WHEN UNBLOCKED).
int	blockedTime  [MAXPID];
int	blockPID     [MAXPID];
int	blockedSector[MAXPID];
int	blockedReason[MAXPID];
int	nBlocked      = 0;

void enqueueBlocked(int usecs, int PID, int sector, int reason)
{
    blockedTime[PID]		= usecs;
    blockPID[nBlocked]		= PID;
    blockedSector[nBlocked]	= sector;
    blockedReason[nBlocked]	= reason;
    ++nBlocked;
}

void dequeueBlocked(int remove)
{
    --nBlocked;
    for(int b=remove ; b<nBlocked ; ++b) {
	blockPID[b]		= blockPID[b+1];
	blockedSector[b]	= blockedSector[b+1];
	blockedReason[b]	= blockedReason[b+1];
    }
}

//  -----------------------------------------------------------------------

int	previousSector	= UNKNOWN;	// the sector last 'seen' by pollDisk

//  DETERMINE THE SECTOR CURRENTLY AVAILABLE  (UNDER THE DISK HEAD)
int whichSector(int usecs)
{
    return ((int)((usecs/1000000.0) * (diskRPM/60.0*diskSectors))) % diskSectors;
}

void pollDisk(int usecs, bool view)
{
    int currentSector = whichSector(usecs);

//  IF SEEING THIS SECTOR FOR THE FIRST TIME, IT'S JUST ARRIVED UNDER THE HEAD
    if(currentSector != previousSector) {

//  CHECK IF ANY BLOCKED PROCESSES WANT THIS SECTOR
	for(int b=0 ; b<nBlocked ; ++b) {
	    if(blockedSector[b] == currentSector) {		// wanted?
		int PID = blockPID[b];

//  PROCESS BLOCKED ON WRITE IS BLOCKED AGAIN TO VERIFY THE SAME SECTOR
		if(blockedReason[b] == IO_WRITE) {
		    blockedReason[b]	= IO_VERIFY;
		    if(view) {
			printf("@%-12i%i\twrite   -> verify\t%i\n",
				usecs, PID, currentSector);
		    }
		    // NOT unblocked
		}
		else {
//  PROCESS BLOCKED ON READ or VERIFY BECOMES READY
		    if(blockedReason[b] == IO_READ) {
			totalBlockedReadTime	+= (usecs - blockedTime[PID]);
		    }
		    else   /* if(blockedReason[b] == IO_VERIFY) */  {
			totalBlockedWriteTime	+= (usecs - blockedTime[PID]);
		    }
		    if(view) {
			printf("@%-12i%i\tblocked -> ready\t%i\n",
				usecs, PID, currentSector);
		    }

//  REMOVE PREVIOUSLY BLOCKED PROCESSES FROM THE QUEUE
		    dequeueBlocked(b);
		    --b;		// careful, we must reconsider this
		    enqueueReady(PID);
		}
	    }
	}
//  REMEMBER THIS SECTOR AS IT'S ONLY 'NEW' ONCE PER ROTATION
	previousSector	= currentSector;
    }
}

//  -----------------------------------------------------------------------

void run_scenario(int nprocs, bool view)
{
    int runningPID	= PID_IDLE;	// the process currently running on CPU
    int quantumUsed	= UNKNOWN;	// usecs consumed by process on CPU

    int nextPID		= 0;		// proc 0 s always admitted first
    int nexited		= 0;		// number of processes that have exited

    int nextevent[nprocs];		// index of each processs' next event
    int individualCPUtime[nprocs];	// each process's total time on CPU

//  INITIALIZE INFORMATION ABOUT EACH PROCESS
    for(int PID=0 ; PID<nprocs ; ++PID) {
	nextevent[PID]		= 0;
	individualCPUtime[PID]	= 0;
    }

//  LOOP UNTIL ALL PROCESSES HAVE EXITED, ADVANCING TIME IN microseconds
    for(int usecs=0 ; nexited < nprocs ; ++usecs) {

//  IF A REQUIRED SECTOR NOW AVAILABLE, MOVE SOME PROCESSES FROM BLOCKED TO READY
	pollDisk(usecs, view);

//  IS IT TIME FOR ANY NEW PROCESSES TO BE ADMITTED?
	while(nextPID < nprocs && usecs == events[nextPID][0]) {
	    nextevent[nextPID]		= 1;
	    enqueueReady(nextPID);
	    if(view) {
		printf("@%-12i%i\tadmit   -> ready\n", usecs, nextPID);
	    }
	    ++nextPID;
	}

//  IS THERE A PROCESS CURRENTLY RUNNING?
	if(runningPID != PID_IDLE) {

//  THE CURRENT PROCESS CONSUMES SOME CPU TIME
	    ++individualCPUtime[runningPID];
	    ++quantumUsed;

//  IS IT NOW TIME FOR THE CURRENT PROCESS'S NEXT EVENT?
	    int next	= nextevent[runningPID];

	    if(individualCPUtime[runningPID] == events[runningPID][next]) {

//  IS THE CURRENT PROCESS EXITING?
		if(events[runningPID][next+1] == HAS_EXITED) {
		    totalTurnaroundTime += (usecs - events[runningPID][0]);
		    ++nexited;
		    if(view) {
			printf("@%-12i%i\trunning -> exit\n", usecs,runningPID);
		    }
		}
//  THE CURRENT PROCESS MUST BE PERFORMING I/O
		else {
		    int sector	= sectors[runningPID][next];
		    int why	= reason [runningPID][next];

		    enqueueBlocked(usecs, runningPID, sector, why);
		    ++nextevent[runningPID];
		    if(view) {
			printf("@%-12i%i\trunning -> %s\t%i\n",
				usecs, runningPID,
				(why == IO_READ) ? "read\t" : "write", sector);
		    }
		}
		runningPID	= PID_IDLE;
	    }

//  HAS THE RUNNING PROCESS USED ITS TIME QUANTUM?
	    else if(quantumUsed == timeQuantum) {

//  IF NOTHING ELSE READY TO RUN, CONTINUE WITH THIS PROCESS
		if(nReady == 0) {
		    quantumUsed = 0;
		    if(view) {
			printf("@%-12i%i\trunning -> running\n",usecs,runningPID);
		    }
		}
//  PRE-EMPT THIS PROCESS, READY FOR ANOTHER ONE
		else {
		    if(view) {
			printf("@%-12i%i\trunning -> ready\n", usecs,runningPID);
		    }
		    enqueueReady(runningPID);
		    runningPID		= PID_IDLE;
		}
	    }
	}

//  NOTHING IS RUNNING, MOVE NEXT READY PROCESS (IF ANY) TO RUNNING
	if(runningPID == PID_IDLE && nReady > 0) {
	    runningPID	= dequeueReady();
	    quantumUsed = 0;
	    if(view) {
		printf("@%-12i%i\tready   -> running\n", usecs,runningPID);
	    }
	}
    }
}

//  -----------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  ENSURE THAT WE HAVE THE REQUIRED NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 2) {
	printf("Usage: %s scenariofile\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    int	 nprocs = read_scenario_file(argv[1]);
    bool view	= (getenv("OSVIEW") != NULL);
    view	= true;

//  SOME INTRO OUTPUT FOR osevents-view
    if(view) {
	printf("# form of events understood by osevents-view:\n#\n");
	printf("#   @usecs  processID  oldState	 ->  newState  [sector]\n#\n");
	printf("@timequantum\t%i\n", timeQuantum);
	printf("@diskrpm\t%i\n", diskRPM);
	printf("@disksectors\t%i\n", diskSectors);
    }

    run_scenario(nprocs, view);

    printf("%i\t%i\t%i\n",
	    totalTurnaroundTime, totalBlockedReadTime, totalBlockedWriteTime);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4
