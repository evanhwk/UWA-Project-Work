/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

//Disk_0.diskQueue[index]
//blockedQueue Disk_0Queue[MAX_PROCESS * MAX_IO_PER_PROCESS];

// PREDECLARING FUNCTIONS 
int calcSectorSeek();

//---------------------------------------------------------------------------------------------------------------------------------//

void printdiskQueue(disk *Disk_0)
{
	//int len = Disk_0->numIOrequests;
	//printf("NUMBER OF ITEMS: %i\n", len);
	printf("QUEUE CURRENT STATUS = [");
	for (int i = 0; i < 10; i++) {
		printf("%i ", Disk_0->diskQueue[i].diskSector);
	}
	printf("]\n");
	printf("%i\n ", Disk_0->diskQueue[50].diskSector);
}

void initDisk(disk *Disk_0)
{
	Disk_0->currSector = 0;
	Disk_0->numIOrequests = 0;
	Disk_0->timeperSector = (SEC_TO_MICS_FACTOR - 1) / ((g_diskRPM/MIN_TO_SEC_FACTOR) * g_numDiskSectors) + 1;	
	printf(" Disk_0->timeperSector %i\n ", Disk_0->timeperSector);
	// Calculate the amount of time needed to advance one sector on the HDD. This value is rounded up. 
	printdiskQueue(Disk_0);
}

void newDiskSeek(disk *Disk_0, int currPID, int currIO, int IORequestSector, char IORequestType)
{
	int timetoNew = calcSectorSeek(Disk_0, IORequestSector);
	int slowestSeek = 0;
	int len = Disk_0->numIOrequests;
	for (int i = 0; i < len; i++)
	{
		int timetoOld = calcSectorSeek(Disk_0->diskQueue[i].diskSector);
		if (timetoNew < timetoOld) 
		{ 
			slowestSeek = i;
		}
	}
	
	for (int i = slowestSeek; i < len; i++)
	{
		Disk_0->diskQueue[i+1] = Disk_0->diskQueue[i];
	}
	Disk_0->numIOrequests++;
	
	Disk_0->diskQueue[slowestSeek].processID = currPID;
	Disk_0->diskQueue[slowestSeek].IOreqID = currIO;
	Disk_0->diskQueue[slowestSeek].operationType = IORequestType;
	Disk_0->diskQueue[slowestSeek].diskSector = IORequestSector;
	//printdiskQueue();
}

void removeDiskSeek(disk *Disk_0)
{
	Disk_0->currSector = Disk_0->diskQueue[0].diskSector;
	int len = Disk_0->numIOrequests;
	for (int i = 0; i < len; i++)
	{
		Disk_0->diskQueue[i] = Disk_0->diskQueue[i+1];
	}
	Disk_0->numIOrequests--;
}

// Queries disk for "estimate" of time until next sector is available
// Done by using function calcSectorSeek to determine time from current sector to target sector as defined by first item in blocked queue.
int queryDisk_nextSector(disk *Disk_0)
{
	if ( Disk_0->numIOrequests > 0) 
	{
		int targetSector = Disk_0->diskQueue[0].diskSector;
		int difference = calcSectorSeek(targetSector);
		
	return  (Disk_0->timeperSector * difference);
	}
	else return INT_MAX;
}

// Queries disk blocked queue for information relating to the first IO request in the queue (Process #/IO Request # it belongs to)
int * queryDisk_infoRecall(disk *Disk_0)
{
	static int result[2]; 
	result[0] = Disk_0->diskQueue[0].processID;
	result[1] = Disk_0->diskQueue[0].IOreqID;
	
	return result;
}

// Investigates difference between currentTime and 
int calcSectorSeek(disk *Disk_0, int targetSector)
{
	int difference = targetSector - Disk_0->currSector;
	if (targetSector - Disk_0->currSector <= 0) 
	{
		difference += g_numDiskSectors;
	}
	return difference;
}