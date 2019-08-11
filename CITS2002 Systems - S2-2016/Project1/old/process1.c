/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// Local Structure Queue
typedef struct
{
	int numItems_inQ;
	int item[MAX_PROCESS];
} Queue;

Queue readyProcessQ;							// Queue structure for handling ready queue
int currTime;									// Variable for handling global time
int numTermProcess;								// Variable for holding number of terminated processes
int currProcessID;								// Variable for holding ID of running process
int nextAdmitCD = INT_MAX;						// Variable for handling countdown until next admit request

void addQueue(Queue, int);
void runProcess(int, int);
void exitProcess(int);
void rotateQueue(Queue);
int findNextEvent();

int runOS() 
{
	
	
	
	currProcessID = 0;									// Initialises current process as process 0
	currTime = processList[currProcessID].admitTime;	// Initialises current time as admit time of process 0
	
	//int checkTime = currTime + timeQuantum;
	processList[currProcessID].state = 'R';
	readyProcessQ.numItems_inQ++;
	addQueue(readyProcessQ, currProcessID);
	
		while (numProcess > numTermProcess) // While there are still processes that have not terminated
		{
			if (currProcessID < MAX_PROCESS) {	// Check to see if any processes need to be admitted
				nextAdmitCD = processList[currProcessID+1].admitTime - currTime; // if yes set a countdown
			}
			else { nextAdmitCD = INT_MAX;} // if no set countdown var to INT_MAX
			
			if ( nextAdmitCD >= timeQuantum) {	// If timequantum is the next event
				if (processList[currProcessID].numIO == 0)	// and if this process has no IO Requests
				{
					if (processList[currProcessID].exitTimeCD >= timeQuantum) 
					{
						/*currTime += timeQuantum;
						processList[currProcessID].processAge += timeQuantum;
						processList[currProcessID].exitTimeCD -= timeQuantum;
						*/
						runProcess(currProcessID, timeQuantum);
						rotateQueue(readyProcessQ);
					}
					else
					{
						/*currTime += processList[currProcessID].exitTimeCD;
						processList[currProcessID].processAge += processList[currProcessID].exitTimeCD;
						*/
						runProcess (currProcessID, processList[currProcessID].exitTimeCD);
						exitProcess(currProcessID);
						numTermProcess++;
					}
				}
			}
		}
	return 0;
}

int findNextEvent()
{
	
}

void runProcess(int pid, int runTime)
{
	currTime += runTime;
	processList[pid].processAge += runTime;
	processList[pid].exitTimeCD -= runTime;
}

void exitProcess(int pid)
{
	//processList[pid].exitTimeCD = 0;
	processList[pid].state = 'X';
	processList[pid].exitTime = currTime;
	readyProcessQ.numItems_inQ--;
	rotateQueue(readyProcessQ);
}

/*void addReadyQueue(int pid)
{
	readyProcessQ.item[readyProcessQ.numItems_inQ] = pid;
	readyProcessQ.numItems_inQ++;
}
*/

void addQueue(Queue Q, int pid)
{
	Q.item[Q.numItems_inQ] = pid;
	Q.numItems_inQ++;
}

void rotateQueue(Queue Q)
{
	if (Q.numItems_inQ == 0) {} else {
		int limit = Q.numItems_inQ - 1;
		int temp = Q.item[0];
		for (int index = 0; index <= limit; index++)
		{
			Q.item[index] = Q.item[index+1];
		}
		Q.item[limit] = temp;
	}
}

/*void rotateReadyQueue()
{
	if (readyProcessQ.numItems_inQ == 0) {} else {
		int limit = readyProcessQ.numItems_inQ - 1;
		int temp = readyProcessQ.item[0];
		for (int index = 0; index <= limit; index++)
		{
			readyProcessQ.item[index] = readyProcessQ.item[index+1];
		}
		readyProcessQ.item[limit] = temp;
	}
}*/