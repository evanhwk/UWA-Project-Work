/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// DECLARING STRUCTURE processQueue
typedef struct
{
	int numProcess_inQ;
	int process[MAX_PROCESS];
} processQueue;

// PREDECLARING FUNCTIONS 
void addQueue(processList *, processQueue *, int);
void rotateQueue(processQueue *);
void removeQueue(processList *, processQueue *, int);
void exitProcess(processList *, processQueue *, int, int);
int * findNextEvent(processList *, processQueue *, int, int, int, int);

// Runs simulation of OS
void runOS(processList *pList) 
{
	int currTime, currPID, admitPID, numTermProcess = 0;					// DECLARING VARIABLES
	currPID = 0;															// Initialises current process as process 0
	admitPID = 1;
	currTime = pList->proc[currPID].admitTime;	    						// Initialises current time as admit time of process 0
	processQueue readyQ, *readyQ_Ptr = &readyQ;							// Queue structure for handling ready process queue
	
	printf("=============================================================================\n");
	
	pList->proc[currPID].state = 'R';
	addQueue(pList, readyQ_Ptr, currPID);
	printf("1 |\t%i\t|\t%i\t| 0 | ADMIT -> RUNNING\n", currTime, currTime);
	bool executing = true;
	while (executing) 
	{
		int *nextEvent = findNextEvent(pList, readyQ_Ptr, currTime, currPID, admitPID, numTermProcess);
		int EventRunTime = nextEvent[1]; //*(nextEvent+1);
				
		switch (*nextEvent) 
		{
			case 0 :
				currTime += EventRunTime;
				pList->proc[currPID].processAge += EventRunTime;
				printf("0 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING\n", currTime, EventRunTime, currPID);
				rotateQueue(readyQ_Ptr);
				currPID = readyQ.process[0];
			break;
			case 1 : 
				currTime += EventRunTime;
				printf("1 |\t%i\t|\t%i\t| %i | ADMIT -> RUNNING\n", currTime, EventRunTime, admitPID);
				addQueue(pList, readyQ_Ptr, admitPID);
				admitPID++;
				currPID = readyQ.process[0];
			break;
			case 2 :
				currTime += EventRunTime;
				pList->proc[currPID].processAge += EventRunTime;
				exitProcess(pList, readyQ_Ptr, currPID, currTime);
				numTermProcess++;
				printf("2 |\t%i\t|\t%i\t| %i | RUNNING -> EXIT %i\n", currTime, EventRunTime, currPID, numTermProcess);
				currPID = readyQ.process[0];
			break;
			case 3 :
				currTime += EventRunTime;													// Advance global time
				pList->proc[currPID].processAge += EventRunTime;							// Record Program Time  (for post processing)
					int currIO = pList->proc[currPID].currIO;								// Collect parameters to call disk function newDiskSeek
					int IORequestSector = pList->proc[currPID].reqIO[currIO].diskSector;
					char IORequestType = pList->proc[currPID].reqIO[currIO].diskSector;
					newDiskSeek(currPID, currIO, IORequestSector, IORequestType);			// Call disk function newDiskSeek
					pList->proc[currPID].currIO++;											// Increase currIO i to address the next read/write IO request
				printf("3 |\t%i\t|\t%i\t| %i | RUNNING -> BLOCKED IO REQUEST %i @ %i\n", currTime, EventRunTime, currPID, currIO, IORequestSector);
				removeQueue(pList, readyQ_Ptr, currPID);												// Remove process from Ready Queue
				currPID = readyQ.process[0];
			break;
			case 4 :
				currTime += EventRunTime;												// Advance global time
				// While we were waiting... 
				if (readyQ.numProcess_inQ >= 1 && pList->proc[currPID].state == 'R' ) {
					pList->proc[currPID].processAge += EventRunTime;
					printf("4 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING (I/O) \n", currTime, EventRunTime, currPID);
				}
				int *record = queryDisk_infoRecall();									// Find which process and IO the data belongs to by calling disk function queryDisk_infoRecall			
				int recordPID = *record;
				int recordIO = *(record+1);
				printf("4 |\t%i\t|\t%i\t| %i | IO REQUEST %i @ %i COMPLETE \n", currTime, EventRunTime, recordPID, recordIO, pList->proc[recordPID].reqIO[recordIO].diskSector);
				
				pList->proc[recordPID].reqIO[recordIO].reqAge += EventRunTime;			// Record Blocked Time (for post processing)
				pList->proc[recordPID].reqIO[recordIO].dataReady = true;				// Set dataReady flag to true
				removeDiskSeek();														// Remove IO request from Blocked Queue
				if (pList->proc[recordPID].reqIO[recordIO].IOType == 'W') {				// If this was a write request
					newDiskSeek(recordPID, recordIO, IORequestSector, 'R'); 			// Create a new read (R) request
					pList->proc[recordPID].reqIO[recordIO].IOType = 'V';				// Set IO Request Type from write (W) to Validate (V)
				}
				else
				{
					pList->proc[recordPID].state = 'R';										// Set process state to Ready
					addQueue(pList, readyQ_Ptr, recordPID);												// Insert it into the ready Queue
					currPID = recordPID;
				}
			break;
			case 5 :
				printf("5 |\t%i\t|\t \t|   | CEASE EXECUTION\n", currTime);
				//PRINT OUT RESULTS
				//END WHILE LOOP
				executing = false;
			break;
		}
	}
	printf("=============================================================================\n");
}

// EVENT 0 		-- Normal operation 
// EVENT 1		-- Admit new Process (admitPID)
// EVENT 2		-- Exit Process N
// EVENT 3		-- Request IO for Process N
// EVENT 4		-- IO request complete for Process N
// EVENT 5 		-- All processes have terminated
int * findNextEvent(processList *pList, processQueue *Q, int currTime, int currPID, int admitPID, int numTermProcess)
{
	int admitCD = INT_MAX;						// Variable for handling countdown until next admit request 
	int exitCD = INT_MAX;						// Variable for handling countdown until next exit request
	int blockCD = INT_MAX;						// Variable for handling countdown until next blocked request
	int unblockCD = INT_MAX;					// Variable for handling countdown until next IO Ready message
												// NOTE: Setting to INT_MAX relates to later usage in calculating which is the closest event
	// Event 5: All Processes Terminated
	
	if (numTermProcess == pList->numProcess)
	{
		static int result[2]; 
		result[0] = 5;
		result[1] = 0;
		return result;
	}
	// Event 1: Admit
	if (currPID < pList->numProcess-1) 
	{
		admitCD = pList->proc[admitPID].admitTime - currTime; 
		if (admitCD < 0) { admitCD = INT_MAX; }

	}
	// Event 2: Exit
	if (pList->proc[currPID].state != 'X')
	{
		exitCD = pList->proc[currPID].processLife - pList->proc[currPID].processAge;
	}
	
	// Event: Request IO (Block)
	if (pList->proc[currPID].numIO != 0) 							// Checks if any IO request exist for this process
	{    
		int currIO = pList->proc[currPID].currIO;
		if (pList->proc[currPID].state == 'R' && !pList->proc[currPID].reqIO[currIO].dataReady) 
		{
			blockCD = pList->proc[currPID].reqIO[currIO].reqTime - pList->proc[currPID].processAge;
		}
		// If so then set a countdown for the IO request/blocked
		//
	}
	
	// FUTURE FUNCTION TO PROBE DISK GOES HERE (return 4)
	// Event: IO Ready
	unblockCD = queryDisk_nextSector();
	
	// FINDING LOWEST COUNTDOWN
	int countdown[] = { g_timeQuantum, admitCD, exitCD, blockCD, unblockCD};
	
		// Single Process only Optimisation
		// Occurs when there is only one process running
		// Possible to disregard timeQuantum until the next admit, exit or IOrequest/IOready event. 
		if (Q->numProcess_inQ <= 1)
		// If there is only one process in the Ready Queue
		{
			countdown[0] = INT_MAX; // Remove timeQuantum as a factor in the calculation below
		}
	
	int lowest = countdown[0];
	int lowesti = 0;
	int len = (sizeof countdown / sizeof countdown[0]);
																					
																					//printf("EVENT 0 %i\n", countdown[0]);
																					
	for (int i = 1; i < len; i++)
	{
		//if (countdown[i] < 0)
		//{
			//countdown[i] = INT_MAX;
		//}
																					//printf("EVENT %i %i\n", i, countdown[i]);
		if (countdown[i] < lowest && countdown[i] >= 0)
		{
			lowesti = i;
			lowest = countdown[i];
		}
	}
	
	static int result[2]; 
	result[0] = lowesti;
	result[1] = lowest;
	
	return result;
}

void addQueue(processList *pList, processQueue *Q, int PID)
{
	pList->proc[PID].state = 'R';
	Q->process[Q->numProcess_inQ] = PID;
	Q->numProcess_inQ++;
	rotateQueue(Q);
}

void rotateQueue(processQueue *Q)
{
	if (Q->numProcess_inQ > 1)
	{
		int limit = Q->numProcess_inQ - 1;
		int temp = Q->process[0];
		for (int i = 0; i <= limit; i++)
		{
			Q->process[i] = Q->process[i+1];
		}
		Q->process[limit] = temp;
	}
}

void removeQueue (processList *pList, processQueue *Q, int PID)
{
	int removeProcess;
	int len = Q->numProcess_inQ;
	
	pList->proc[PID].state = 'B';										// Set State to Blocked

	for (int i = 0; i < len; i++)
	{
		if (Q->process[i] == PID)
		{
			removeProcess = i;
			break;
		}
	}
	len -= 1;
	for (int i = removeProcess; i < len; i++)
	{
		Q->process[i] = Q->process[i+1];
	}
	Q->numProcess_inQ--;
}

void exitProcess(processList *pList, processQueue *Q, int PID, int currTime)
{
	removeQueue(pList, Q, PID);
	pList->proc[PID].state = 'X';
	pList->proc[PID].exitTime = currTime;
}

void calculateOStime(processList *pList)
{
	int len = pList->numProcess;
	int sumProcess = 0;
	for (int i = 0; i < len; i++)
	{
		sumProcess += ( pList->proc[i].exitTime - pList->proc[i].admitTime);
	}

	int sumRead = 0;
	for (int i = 0; i < len; i++)
	{
		int lenJ = pList->proc[i].numIO;
		for (int j = 0; j < lenJ; j++)
		{
			if (pList->proc[i].reqIO[j].IOType == 'R') {
				sumRead += pList->proc[i].reqIO[j].reqAge;
			}
		}
	}

	int sumWrite = 0;
	for (int i = 0; i < len; i++)
	{
		int lenJ = pList->proc[i].numIO;
		for (int j = 0; j < lenJ; j++)
		{
			if (pList->proc[i].reqIO[j].IOType == 'V' || pList->proc[i].reqIO[j].IOType == 'W') {
				sumWrite += pList->proc[i].reqIO[j].reqAge;
			}
		}
	}
	printf("%i\t%i\t%i\n", sumProcess, sumRead, sumWrite);
}