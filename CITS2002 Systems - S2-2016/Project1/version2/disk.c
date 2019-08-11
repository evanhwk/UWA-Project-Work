/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

//Disk_0.diskQueue[index]
blockedQueue Disk_0Queue[MAX_PROCESS * MAX_IO_PER_PROCESS];
int currSector, numIOrequests, timeperSector;

// PREDECLARING FUNCTIONS 
int calcSectorSeek();

//---------------------------------------------------------------------------------------------------------------------------------//

void initDisk()
{
	currSector = 0;
	numIOrequests = 0;
	timeperSector = (SEC_TO_MICS_FACTOR - 1) / ((g_diskRPM/MIN_TO_SEC_FACTOR) * g_numDiskSectors) + 1;	
	// Calculate the amount of time needed to advance one sector on the HDD. This value is rounded up. 
}

void newDiskSeek (int currPID, int currIO, int IORequestSector, char IORequestType)
{
	int timetoNew = calcSectorSeek(IORequestSector);
	int slowestSeek = 0;
	int len = numIOrequests;
	for (int i = 0; i < len; i++)
	{
		int timetoOld = calcSectorSeek(Disk_0Queue[i].diskSector);
		if (timetoNew < timetoOld) 
		{ 
			slowestSeek = i;
		}
	}
	
	for (int i = slowestSeek; i < len; i++)
	{
		Disk_0Queue[i+1] = Disk_0Queue[i];
	}
	numIOrequests++;
	
	Disk_0Queue[slowestSeek].processID = currPID;
	Disk_0Queue[slowestSeek].IOreqID = currIO;
	Disk_0Queue[slowestSeek].operationType = IORequestType;
	Disk_0Queue[slowestSeek].diskSector = IORequestSector;
	//printdiskQueue();
}

void removeDiskSeek()
{
	currSector = Disk_0Queue[0].diskSector;
	int len = numIOrequests;
	for (int i = 0; i < len; i++)
	{
		Disk_0Queue[i] = Disk_0Queue[i+1];
	}
	numIOrequests--;
}

// Queries disk for "estimate" of time until next sector is available
// Done by using function calcSectorSeek to determine time from current sector to target sector as defined by first item in blocked queue.
int queryDisk_nextSector()
{
	if ( numIOrequests > 0) 
	{
		int targetSector = Disk_0Queue[0].diskSector;
		int difference = calcSectorSeek(targetSector);
		
		return  (timeperSector * difference);
	}
	else return INT_MAX;
}

// Queries disk blocked queue for information relating to the first IO request in the queue (Process #/IO Request # it belongs to)
int * queryDisk_infoRecall()
{
	static int result[2]; 
	result[0] = Disk_0Queue[0].processID;
	result[1] = Disk_0Queue[0].IOreqID;
	
	return result;
}

// Investigates difference between currentTime and 
int calcSectorSeek(int targetSector)
{
	int difference = targetSector - currSector;
	if (targetSector - currSector <= 0) 
	{
		difference += g_numDiskSectors;
	}
	return difference;
}