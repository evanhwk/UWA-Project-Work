/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		22/09/2016
*/
#include "osevents.h"

// DECLARING STRUCTURE processQueue (scope limited to os.c)
typedef struct
{
	int numProcess_inQ;														// Length of this array of process IDs (How big the array is)
	int process[MAX_PROCESS];												// Array of process IDs
} processQueue;

// PREDECLARING FUNCTIONS 
void addQueue(processList *, processQueue *, int);
void rotateQueue(processQueue *);
void removeQueue(processList *, processQueue *, int);
void exitProgram(processList *, processQueue *, int, int);
int * findNextEvent(processList *, processQueue *, int, int, int, int);

// Runs simulation of OS - First skips ahead to event ADMIT 0 then calls findNextEvent() to find when the next event will occur. 
// Based on the return value of findNextEvent, execute different set of instructions (based on 6 cases - running/admit/exit/read/write/terminate)
// Function call to findNextEvent() and switch statement are looped until findNextEvent() returns value indicating that no events are left.
void runOS(processList *pList) 
{																		// Time jumps ahead to when the first process is admitted (no need to simulate idle OS kernel)
	int currTime, currPID, admitPID, TermProcessCount = 0;					// Variables handling current time, current running process #, next process (admit event) #, count of terminated processes
	currPID = 0;															// Initialises current process as process 0 and the next process to be admitted as process 1
	admitPID = 1;															// Current time is the admit time of process 0
	currTime = pList->proc[currPID].admitTime;	    						
	processQueue readyQ, *readyQ_Ptr = &readyQ;								// Queue structure (linked to pointer) for handling ready queue (osevents.h)
	addQueue(pList, readyQ_Ptr, currPID);									// Calls addQueue() to add process 0 to ReadyQueue and set state to Ready (R)
	
	printf("=============================================================================\n");	
	printf("E |\tcurrTime|\teventTim| # | Event Info\n");	
	printf("=============================================================================\n");
	printf("1 |\t%i\t|\t%i\t| 0 | ADMIT \n", currTime, currTime);
	
	bool executing = true;													// Loop control boolean (only returns false when all events conclude)
	while (executing) 
	{
		int *nextEvent = findNextEvent(pList, readyQ_Ptr, currTime, currPID, admitPID, TermProcessCount);	// calls findNextEvent() based on current data - returns {nextEvent, timeData}
		int EventRunTime = nextEvent[1]; //*(nextEvent+1);													// Record timedata return from findNextEvent()
				
		switch (*nextEvent) 												// Evaluates return value and executes instructions corresponding to event type
		{																	// See findNextEvent for more detailed notes on these 6 possible execution cases
			case 0 :														// RUNNING/NORMAL EXECUTION
				currTime += EventRunTime;										// Advance time
				pList->proc[currPID].processAge += EventRunTime;				// Increase age of currently running process (for post processing)
				printf("0 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING\n", currTime, EventRunTime, currPID); 
				rotateQueue(readyQ_Ptr);										// -- EXECUTION ENDS after time quantum exceeded , ready queue is rotated
				currPID = readyQ.process[0];									// Set the currentID holding variable to the next process in ready queue
			break;
			case 1 : 														// ADMIT NEW PROCESS
				currTime += EventRunTime;											// Advance time
				printf("1 |\t%i\t|\t%i\t| %i | ADMIT \n", currTime, EventRunTime, admitPID);
				addQueue(pList, readyQ_Ptr, admitPID);								// Add new process to the beginning of the Queue 
				admitPID++;															// Set next process to be admitted as previous highest process + 1
				currPID = readyQ.process[0];										// Set the currentID holding variable as the newly admitted process
			break;
			case 2 :														// EXIT CURRENT PROCESS
				currTime += EventRunTime;											// Advance time
				pList->proc[currPID].processAge += EventRunTime;					// Increase age of currently running process (for post processing)
				exitProgram(pList, readyQ_Ptr, currPID, currTime);					// Calls ExitProgram() to remove process from readyQueue and set state to exit (X)
				TermProcessCount++;													// Increases terminated process count
				printf("2 |\t%i\t|\t%i\t| %i | RUNNING -> EXIT ( EXIT COUNT: %i )\n", currTime, EventRunTime, currPID, TermProcessCount);
				currPID = readyQ.process[0];										// Sets the currentID holding variable to the next process in ready queue
			break;
			case 3 :														// IO REQUEST - PROCESS BLOCKED
				currTime += EventRunTime;													// Advance time
				pList->proc[currPID].processAge += EventRunTime;							// Record Program Time  (for post processing)
					int currIO = pList->proc[currPID].currIO;								// Collect parameters to call disk function newDiskSeek
					int IORequestSector = pList->proc[currPID].reqIO[currIO].diskSector;	
					char IORequestType = pList->proc[currPID].reqIO[currIO].IOType;
					printf("3 |\t%i\t|\t%i\t| %i | RUNNING -> BLOCKED I/O %c REQUEST %i @ %i\n", currTime, EventRunTime, currPID, IORequestType, currIO, IORequestSector);
					newDiskSeek(currPID, currIO, IORequestSector, IORequestType);			// Call disk function newDiskSeek() to create a new entry in the Disk_0 blocked queue
					pList->proc[currPID].currIO++;											// Increase currIO index to address the next read/write IO request (for this process)
				removeQueue(pList, readyQ_Ptr, currPID);									// Remove process from Ready Queue
				currPID = readyQ.process[0];												// Sets the currentID holding variable to the next process in ready queue
			break;
			case 4 :														// IO MESSAGE - DATA IS READY
				currTime += EventRunTime;												// Advance time
				if (readyQ.numProcess_inQ >= 1 && pList->proc[currPID].state == 'R' ) {	// Was there a process that was Ready in the ready queue during block request? If Y, execute.
					pList->proc[currPID].processAge += EventRunTime;					// Increase age of this process (for post processing)
					printf("4 |\t%i\t|\t%i\t| %i | RUNNING -> RUNNING (I/O) \n", currTime, EventRunTime, currPID);
				}
				int *record = getNextIORequest();									// Find which process and IO the data belongs to by calling disk function getNextIORequest			
				int recordPID = record[0];											// Record the process ID and IO request ID from return value	
				int recordIO = record[1];
				printf("4 |\t%i\t|\t%i\t| %i | I/O %c REQUEST %i @ %i COMPLETE \n", currTime, EventRunTime, recordPID, pList->proc[recordPID].reqIO[recordIO].IOType, recordIO, pList->proc[recordPID].reqIO[recordIO].diskSector);
				
				pList->proc[recordPID].reqIO[recordIO].reqAge += EventRunTime;			// Record Blocked Time (for post processing)
				pList->proc[recordPID].reqIO[recordIO].dataReady = true;				// Set dataReady flag to true
				removeDiskSeek();														// Remove IO request from Blocked Queue
				if (pList->proc[recordPID].reqIO[recordIO].IOType == 'W') {			// If this was a write request
					newDiskSeek(recordPID, recordIO, IORequestSector, 'B'); 			// Create a new blocked (B) request to disk - Has to rotate around the disk once
					pList->proc[recordPID].reqIO[recordIO].IOType = 'V';				// Set IO Request Type from write (W) to Validate (V)
				}
				else																// If this was a read request/validation request
				{
					pList->proc[recordPID].state = 'R';										// Set process state to Ready
					addQueue(pList, readyQ_Ptr, recordPID);									// Insert it into the ready Queue
					currPID = recordPID;													// Set currentID holding variable to this process
				}
			break;
			case 5 :
				printf("5 |\t%i\t|\t \t|   | CEASE EXECUTION\n", currTime);
				executing = false;
			break;
		}
	}
	printf("=============================================================================\n");
}

// Based on data passed from runOS i.e. current time, current process (in ready Queue), next admitted process and # of terminated processes
// First calculate the time delta between the following events, then select the lowest time delta event as the next event.
// EVENT 0 		-- Normal operation with ready Queue switching (based on timeQuantum)
// EVENT 1		-- Admit new Process (admitPID)													
// EVENT 2		-- Exit Process N																
// EVENT 3		-- Request IO for Process N														
// EVENT 4		-- IO request complete for Process N											
// EVENT 5 		-- All processes have terminated
int * findNextEvent(processList *pList, processQueue *Q, int currTime, int currPID, int admitPID, int TermProcessCount)
{
	int timeQuantum = INT_MAX;																// Declare variables for handling time delta until next event (of each type)
	int admitTD = INT_MAX;																	
	int exitTD = INT_MAX;																	
	int blockTD = INT_MAX;																	
	int unblockTD = INT_MAX;																
																							// NOTE: Setting to INT_MAX relates to later usage in calculating which is the closest event	
																					// EVENT 5: Terminate Program																			
	if (TermProcessCount == pList->numProcess) {											// If terminated process count is equal to total count of processes
		static int result[2]; 																
		result[0] = 5;																		// Set return values
		result[1] = 0;																	
		return result;																		// Return values and break out of this function.
	}																						// Past this point disregard event 5 as a possible next event.
																					// EVENT 0: Running/Normal Operation
	if (Q->numProcess_inQ > 0){																// If there is at least one process in running queue
		timeQuantum = getTimeQuantum(); 													// Record timequantum (from init) by calling getTimeQuantum()
	}
																					// EVENT 1: Admit
	if (admitPID < pList->numProcess) {														// If this is the last process to be admitted, disregard event 1 as a possible next event																		
		admitTD = pList->proc[admitPID].admitTime - currTime; 								// Record admitTD as time delta between current time and admission of next process								
	}
																					// EVENT 2: Exit
	if (pList->proc[currPID].state != 'X'){													// If the current process has not already exited
		exitTD = pList->proc[currPID].processLife - pList->proc[currPID].processAge;		// Record exitTD as time delta between current time and exiting of current process
	}
																					// EVENT 3: Request IO (Block)
	if (pList->proc[currPID].currIO < pList->proc[currPID].numIO) {  								// Checks if any untouched IO requests exist for this process
		int currIO = pList->proc[currPID].currIO;											
		if (pList->proc[currPID].state == 'R' && !pList->proc[currPID].reqIO[currIO].dataReady)		 // If the process state is Ready (R) and its latest IO request has not been fulfilled
		{
			blockTD = pList->proc[currPID].reqIO[currIO].reqTime - pList->proc[currPID].processAge;	 // Record blockTD as time delta between current time and next blocked request
		}
	}
																					// EVENT 4: IO Ready -- this is a possible cause of errors as it is possible for admit, 
																					// I/O request and exit events to have a LOWER delta than the time taken to advance the disk by one sector. 
																					// (~100-200 microseconds based on RPM) The disk remains idle whilst it could have been spinning.
	unblockTD = queryDisk();															// Calls method queryDisk(), which returns a result based on current disk position and the first blocked item's data request.
	
	// FINDING LOWEST TIME DELTA
	int timeDelta[] = { timeQuantum, admitTD, exitTD, blockTD, unblockTD};			// First build an array timeDelta using the values calculated above
	int lowest = timeDelta[0];															// Set lowest value/index to timeQuantum (event 0)
	int lowestindex = 0;																
	int len = (sizeof timeDelta / sizeof timeDelta[0]);
	for (int i = 1; i < len; i++) {													// Loop over the array	
		if (timeDelta[i] < lowest && timeDelta[i] >= 0) {								// If the current TD is lower than the current lowest TD
			lowestindex = i;															// This is now the lowest TD
			lowest = timeDelta[i];
		}
		if (timeDelta[i] < 0) {														// For debugging - If any data in the array is negative, print data to console
			printf("=============================================================================\n");
			printf("Possible Error Detected\n");
			printf("EVENT %i %i\n", i, timeDelta[i]);
			printf("=============================================================================\n");
		}
	}
	static int result[2]; 															// Put result into an array to be returned
	result[0] = lowestindex;														// Format: {eventID, eventTime}
	result[1] = lowest;
	
	return result;
}

// Insert process ID PID into the front of the readyQueue, and set the process's state to Ready
// Also increase the index recording number of processes in the ready queue
void addQueue(processList *pList, processQueue *Q, int PID)							
{
	pList->proc[PID].state = 'R';													// Setting state of PID to Ready					
	Q->process[Q->numProcess_inQ] = PID;											// Inserting PID into the BACK of the array (past array's current limit)
	Q->numProcess_inQ++;															// Increasing array index by 1
	rotateQueue(Q);																	// ROTATE the queue so that PID is now at the front of the array (calling rotateQueue())
}

// Rotates the queue so that the last element of the array is moved to the front, with every other item shifted one down as a result.
void rotateQueue(processQueue *Q)
{
	if (Q->numProcess_inQ > 1) {													// If there is more than 1 item in the array
		int limit = Q->numProcess_inQ - 1;											
		int temp = Q->process[0];
		for (int i = 0; i <= limit; i++) {											// Loop through the array
			Q->process[i] = Q->process[i+1];										// Moving every element to the right
		}
		Q->process[limit] = temp;													// Then copy the former last element into the first element of the array
	}
}

// Find the processID PID requested in the readyQueue and remove it from the readyQueue. 
void removeQueue (processList *pList, processQueue *Q, int PID)
{
	
	pList->proc[PID].state = 'B';										// Set State to Blocked
	
	int removeProcess;
	int len = Q->numProcess_inQ;
	for (int i = 0; i < len; i++){										// Loops through the array looking for PID
		if (Q->process[i] == PID){										// If the current indexed element is PID
			removeProcess = i;												// Record the index
			break;															// Break out of the loop
		}
	}
	len -= 1;															// Recycle loop control var -1
	for (int i = removeProcess; i < len; i++)							// Loop through the array beginning from the recorded index
	{
		Q->process[i] = Q->process[i+1];								// Move every element left (overrides the originally indexed element)
	}
	Q->numProcess_inQ--;
}

// Called upon process termination. Removes Process PID from readyQueue, sets state to exit (X) and records the process's termination time (for post processing)
void exitProgram(processList *pList, processQueue *Q, int PID, int currTime)
{
	removeQueue(pList, Q, PID);											// calls removeQueue()
	pList->proc[PID].state = 'X';										// set state to exit (X)
	pList->proc[PID].exitTime = currTime;								// record exit time as current time
}

//Post process calculations for total process execution time, total read blocked time and total write blocked time
void calculateOStime(processList *pList)
{
	int len = pList->numProcess;
	int sumProcess = 0;															// init sumProcess
	for (int i = 0; i < len; i++) {												// Loop over the entire array of processes in processList
		sumProcess += ( pList->proc[i].exitTime - pList->proc[i].admitTime);	// Add all of the elements of exitTime - admitTime together to get total running time
	}

	int sumRead = 0;															// init sumRead
	for (int i = 0; i < len; i++) {												// Loop over the entire array of processes
		int lenJ = pList->proc[i].numIO;
		for (int j = 0; j < lenJ; j++) {										// Within each process loop over each array of IO requests
			if (pList->proc[i].reqIO[j].IOType == 'R') {						// Only consider IO requests with a type of Read (R)
				sumRead += pList->proc[i].reqIO[j].reqAge;						// Add all of the elements of reqAge together 
			}
		}
	}

	int sumWrite = 0;															// init sumWrite
	for (int i = 0; i < len; i++) {												// Loop over the entire array of processes
		int lenJ = pList->proc[i].numIO;
		for (int j = 0; j < lenJ; j++)											// Within each process loop over each array of IO requests
		{
			if (pList->proc[i].reqIO[j].IOType == 'V') {						// Only consider IO requests with a type of Verified Write (V)
				sumWrite += pList->proc[i].reqIO[j].reqAge;						// Add all of the elements of reqAge together 
			}
		}
	}
	printf("%i\t%i\t%i\n", sumProcess, sumRead, sumWrite);						// Print out the values
}