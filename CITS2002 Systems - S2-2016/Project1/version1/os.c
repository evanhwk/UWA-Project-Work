/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// Declaring Structure processQueue (limited in scope to process.c)
typedef struct
{
	int numProcess_inQ;
	int process[MAX_PROCESS];
} processQueue;

// PREDECLARING FUNCTIONS 

void addQueue(processQueue *, int);
void rotateQueue(processQueue *);
void removeQueue(processQueue *, int);
void exitProcess(processQueue *, int, int);
int * findNextEvent(int, int, int, int, processQueue *);

// Runs simulation of OS
void runOS() 
{
	int currTime, currPID, admitPID, numTermProcess = 0;					// DECLARING VARIABLES
	currPID = 0;															// Initialises current process as process 0
	admitPID = 1;
	currTime = processList[currPID].admitTime;	    						// Initialises current time as admit time of process 0
	processQueue readyQ, *readyQ_Ptr = &readyQ;							// Queue structure for handling ready process queue
	
	printf("=============================================================================\n");
	
	processList[currPID].state = 'R';
	addQueue(readyQ_Ptr, currPID);
	printf("1 |\t%i\t|\t%i\t| 0 | ADMIT -> RUNNING\n", currTime, currTime);
	bool executing = true;
	while (executing) 
	{
		int *nextEvent = findNextEvent(currTime, currPID, admitPID, numTermProcess, readyQ_Ptr);
		int EventRunTime = *(nextEvent+1);
				
		switch (*nextEvent) 
		{
			case 0 :
				currTime += EventRunTime;
				processList[currPID].processAge += EventRunTime;
				printf("0 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING\n", currTime, EventRunTime, currPID);
				rotateQueue(readyQ_Ptr);
				currPID = readyQ.process[0];
			break;
			case 1 : 
				currTime += EventRunTime;
				printf("1 |\t%i\t|\t%i\t| %i | ADMIT -> RUNNING\n", currTime, EventRunTime, admitPID);
				addQueue(readyQ_Ptr, admitPID);
				admitPID++;
				currPID = readyQ.process[0];
			break;
			case 2 :
				currTime += EventRunTime;
				processList[currPID].processAge += EventRunTime;
				exitProcess(readyQ_Ptr, currPID, currTime);
				numTermProcess++;
				printf("2 |\t%i\t|\t%i\t| %i | RUNNING -> EXIT %i\n", currTime, EventRunTime, currPID, numTermProcess);
				currPID = readyQ.process[0];
			break;
			case 3 :
				currTime += EventRunTime;													// Advance global time
				processList[currPID].processAge += EventRunTime;							// Record Program Time  (for post processing)
					int currIO = processList[currPID].currIO;								// Collect parameters to call disk function newDiskSeek
					int IORequestSector = processList[currPID].reqIO[currIO].diskSector;
					char IORequestType = processList[currPID].reqIO[currIO].diskSector;
					newDiskSeek(currPID, currIO, IORequestSector, IORequestType);			// Call disk function newDiskSeek
					processList[currPID].currIO++;											// Increase currIO index to address the next read/write IO request
				printf("3 |\t%i\t|\t%i\t| %i | RUNNING -> BLOCKED IO REQUEST %i @ %i\n", currTime, EventRunTime, currPID, currIO, IORequestSector);
				removeQueue(readyQ_Ptr, currPID);												// Remove process from Ready Queue
				currPID = readyQ.process[0];
			break;
			case 4 :
				currTime += EventRunTime;												// Advance global time
				// While we were waiting... 
				if (readyQ.numProcess_inQ >= 1 && processList[currPID].state == 'R' ) {
					processList[currPID].processAge += EventRunTime;
					printf("4 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING (I/O) \n", currTime, EventRunTime, currPID);
				}
				int *record = queryDisk_infoRecall();									// Find which process and IO the data belongs to by calling disk function queryDisk_infoRecall			
				int recordPID = *record;
				int recordIO = *(record+1);
				printf("4 |\t%i\t|\t%i\t| %i | IO REQUEST %i @ %i COMPLETE \n", currTime, EventRunTime, recordPID, recordIO, processList[recordPID].reqIO[recordIO].diskSector);
				
				processList[recordPID].reqIO[recordIO].reqAge += EventRunTime;			// Record Blocked Time (for post processing)
				processList[recordPID].reqIO[recordIO].dataReady = true;				// Set dataReady flag to true
				removeDiskSeek();														// Remove IO request from Blocked Queue
				if (processList[recordPID].reqIO[recordIO].IOType == 'W') {				// If this was a write request
					newDiskSeek(recordPID, recordIO, IORequestSector, 'R'); 			// Create a new read request
					processList[recordPID].reqIO[recordIO].IOType = 'V';				// Set IO Request Type to Validate
				}
				else
				{
					processList[recordPID].state = 'R';										// Set process state to Ready
					addQueue(readyQ_Ptr, recordPID);												// Insert it into the ready Queue
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
int * findNextEvent(int currTime, int currPID, int admitPID, int numTermProcess, processQueue *Q)
{
	int admitCD = INT_MAX;						// Variable for handling countdown until next admit request 
	int exitCD = INT_MAX;						// Variable for handling countdown until next exit request
	int blockCD = INT_MAX;						// Variable for handling countdown until next blocked request
	int unblockCD = INT_MAX;					// Variable for handling countdown until next IO Ready message
												// NOTE: Setting to INT_MAX relates to later usage in calculating which is the closest event
	// Event 5: All Processes Terminated
	
	if (numTermProcess == g_numProcess)
	{
		static int result[2]; 
		result[0] = 5;
		result[1] = 0;
		return result;
	}
	// Event 1: Admit
	if (currPID < g_numProcess-1) 
	{
		admitCD = processList[admitPID].admitTime - currTime; 
		if (admitCD < 0) { admitCD = INT_MAX; }

	}
	// Event 2: Exit
	if (processList[currPID].state != 'X')
	{
		exitCD = processList[currPID].processLife - processList[currPID].processAge;
	}
	
	// Event: Request IO (Block)
	if (processList[currPID].numIO != 0) 							// Checks if any IO request exist for this process
	{    
		int currIO = processList[currPID].currIO;
		if (processList[currPID].state == 'R' && !processList[currPID].reqIO[currIO].dataReady) 
		{
			blockCD = processList[currPID].reqIO[currIO].reqTime -  processList[currPID].processAge;
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
	int lowestIndex = 0;
	int length = (sizeof countdown / sizeof countdown[0]);
																					
																					//printf("EVENT 0 %i\n", countdown[0]);
																					
	for (int index = 1; index < length; index++)
	{
		//if (countdown[index] < 0)
		//{
			//countdown[index] = INT_MAX;
		//}
																					//printf("EVENT %i %i\n", index, countdown[index]);
		if (countdown[index] < lowest && countdown[index] >= 0)
		{
			lowestIndex = index;
			lowest = countdown[index];
		}
	}
	
	static int result[2]; 
	result[0] = lowestIndex;
	result[1] = lowest;
	
	return result;
}

void addQueue(processQueue *Q, int PID)
{
	processList[PID].state = 'R';
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
		for (int index = 0; index <= limit; index++)
		{
			Q->process[index] = Q->process[index+1];
		}
		Q->process[limit] = temp;
	}
}

void removeQueue (processQueue *Q, int PID)
{
	int removeProcess;
	int length = Q->numProcess_inQ;
	
	processList[PID].state = 'B';										// Set State to Blocked

	for (int index = 0; index < length; index++)
	{
		if (Q->process[index] == PID)
		{
			removeProcess = index;
			break;
		}
	}
	length -= 1;
	for (int index = removeProcess; index < length; index++)
	{
		Q->process[index] = Q->process[index+1];
	}
	Q->numProcess_inQ--;
}

void exitProcess(processQueue *Q, int PID, int currTime)
{
	removeQueue(Q, PID);
	processList[PID].state = 'X';
	processList[PID].exitTime = currTime;
}

void calculateOStime()
{
	int length = g_numProcess;
	int sumProcess = 0;
	for (int i = 0; i < length; i++)
	{
		sumProcess += ( processList[i].exitTime - processList[i].admitTime);
	}

	int sumRead = 0;
	for (int i = 0; i < length; i++)
	{
		int lengthJ = processList[i].numIO;
		for (int j = 0; j < lengthJ; j++)
		{
			if (processList[i].reqIO[j].IOType == 'R') {
				sumRead += processList[i].reqIO[j].reqAge;
			}
		}
	}

	int sumWrite = 0;
	for (int i = 0; i < length; i++)
	{
		int lengthJ = processList[i].numIO;
		for (int j = 0; j < lengthJ; j++)
		{
			if (processList[i].reqIO[j].IOType == 'V' || processList[i].reqIO[j].IOType == 'W') {
				sumWrite += processList[i].reqIO[j].reqAge;
			}
		}
	}
	printf("%i\t%i\t%i\n", sumProcess, sumRead, sumWrite);
}