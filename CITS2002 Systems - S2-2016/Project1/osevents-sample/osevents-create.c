#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

//  osevents-create.c, written by Chris.McDonald@uwa.edu.au, September 2016
//  compile with:  cc -std=c99 -Wall -Werror -pedantic -o osevents osevents.c

#define MAXPROCS	50
#define MAXIOS		30

#define EV_ADMIT	0
#define EV_EXIT		1

#define UNSTARTED	0
#define DEAD		(-1)

#define FOREACH_PROC	for(int PID=0 ; PID<nprocs ; ++PID)
#define IS_RUNNING(p)	(started[p] > UNSTARTED)

//  -----------------------------------------------------------------------

static void create(int nprocs)
{
    if(nprocs > MAXPROCS)
	nprocs	= MAXPROCS;

    int nrunning	= 0;

    int *started	= malloc(nprocs * sizeof(started[0]));
    int *nios		= malloc(MAXIOS * sizeof(nios[0]));

    FOREACH_PROC {
	started[PID]	= UNSTARTED;
	nios[PID]	= rand() % MAXIOS;
    }

//  GENERATE AND PRINT THE SYSTEM'S CHARACTERISTICS IN A RANDOM ORDER
    int timequantum	= ((rand() % 10)+1) * 2000;
    int rpms[]		= { 4200, 5400, 7200 };
    int diskrpm		= rpms[rand() % (sizeof(rpms) / sizeof(rpms[0]))];
    int sectors[]	= { 24, 30, 48, 60, 72 };
    int disksectors	= sectors[rand() % (sizeof(sectors)/sizeof(sectors[0]))];

    int order[] = { 0, 1, 2 };
    int nleft	= 3;

    while(nleft > 0) {
	int save	= order[nleft-1];
	int choose	= rand() % nleft;

	order[nleft-1]	= order[choose];
	order[choose]	= save;
	nleft		= nleft-1;
    }
    for(int i=0 ; i<3 ; ++i) {
	switch (order[i]) {
	    case 0: printf("timequantum\t%i\n", timequantum);	break;
	    case 1: printf("diskrpm\t\t%i\n", diskrpm);		break;
	    case 2: printf("disksectors\t%i\n", disksectors);	break;
	}
    }

//  GENERATE THE EVENT TRACE
    for(int usec=1000 ; (nrunning>0 || started[nprocs-1] != DEAD) ; usec += 100)
    {
	int	r = rand()%1000;

	if(r < 50) switch (r) {

	case EV_ADMIT:
	    FOREACH_PROC {
		if(started[PID] == UNSTARTED) {
		    started[PID] = usec;
		    printf("%i\t%i\tadmit\n", started[PID], PID);
		    ++nrunning;
		    break;
		}
	    }
	    break;

	case EV_EXIT:
	    if(nrunning > 0) {
		int	try = rand() % nprocs;

		FOREACH_PROC {
		    if(IS_RUNNING(try)) {
			printf("%i\t%i\texit\n", usec-started[try], try);
			started[try]	= DEAD;
			--nrunning;
			break;
		    }
		    try = (try+1)%nprocs;
		}
	    }
	    break;

	// case EV_IO :
	default:
	    if(nrunning > 0) {
		int	try = rand() % nprocs;

		FOREACH_PROC {
		    if(IS_RUNNING(try) && nios[try] > 0) {
			--nios[try];
			printf("%i\t%i\t%s\t%i\n",
				usec-started[try], try,
				((rand() % 2) ? "read" : "write"),
				rand() % disksectors);
			break;
		    }
		    try = (try+1)%nprocs;
		}
	    }
	    break;
	}
    }
}

//  -----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int nprocs		= 2;
    int seed		= getpid();

    if(argc > 1) {
	nprocs	= atoi(argv[1]);
	if(argc > 2)
	    seed	= atoi(argv[2]);
    }

    if(nprocs < 1 || seed < 0) {
	fprintf(stderr, "Usage: %s [nprocs]\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    srand(seed);
    create(nprocs);
    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4
