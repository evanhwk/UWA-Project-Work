/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// DECLARING STRUCTURE blockedQueue (scope limited to disk.c)
typedef struct
{
	int processID, IOreqID, diskSector;
	char operationType;
} blockedQueue;

// DECLARING FILE SCOPE VARIABLES
blockedQueue Disk_0Queue[MAX_PROCESS * MAX_IO_PER_PROCESS];								//Create BlockedQueue for Disk_0 aka the hard disk drive
static int timeperSector, currSector, numDiskSectors, numIORequests;					//Time taken to move from one sector to the next, current sector, total number of disk sectors, total number of I/O requests

// PREDECLARING FUNCTIONS 
int calcSectorSeek(int, bool);

//---------------------------------------------------------------------------------------------------------------------------------//

// Initiate the file scope variables (after receiving information from scenario)
void initDisk()
{
	currSector = 0;										// Read/Write Head begins at Sector 0
	numIORequests = 0;
	numDiskSectors = getNumDiskSectors();				// Call function getNumDiskSectors() (init.c)
	timeperSector = (SEC_TO_MICS_FACTOR - 1) / ( (getDiskRPM()/MIN_TO_SEC_FACTOR) * getNumDiskSectors() ) + 1;	
	// Calculate the amount of time needed to advance one sector on the HDD. This value is rounded up. 
	// Calls getDiskRPM() and getNumDiskSectors() from init.c
}

// Create a new seek request (into the blocked queue). New request is placed into the queue based on 
void newDiskSeek (int currPID, int currIO, int IORequestSector, char IORequestType)
{
	numIORequests++;											// Increase array index
	int currDelta = calcSectorSeek(IORequestSector, false);		// Calculate the difference between current sector and new the new DiskSeek request's sector, given that this value cannot be 0. 
	int i = numIORequests - 1;									// Set index to last (currently valid) value (aka 2nd last value of current array)
	while ((i > 0) && ( currDelta < calcSectorSeek(Disk_0Queue[i-1].diskSector, false)))		// Loops through array from the back, until the delta of index-1 is bigger than the current delta
	{
		Disk_0Queue[i] = Disk_0Queue[i-1];						// While we look for the index value, move all the items we processed in the queue to the right
		i--;
	}
	Disk_0Queue[i].processID = currPID;							// Init the new disk seek request into the blocked queue, now that we have our index.
	Disk_0Queue[i].IOreqID = currIO;							
	Disk_0Queue[i].operationType = IORequestType;
	Disk_0Queue[i].diskSector = IORequestSector;
}

// Remove an old seek request (from the blocked queue)
void removeDiskSeek()										
{
	currSector = Disk_0Queue[0].diskSector;						// Use this as an opportunity to advance the current sector 
	int len = numIORequests;									
	for (int i = 0; i < len; i++)								// Loop through array, moving items to the left
	{
		Disk_0Queue[i] = Disk_0Queue[i+1];
	}
	numIORequests--;											// Decrease array index
}

// Queries disk for "estimate" of time until next I/O ready message is sent
// Done by using function calcSectorSeek to determine time from current sector to target sector as defined by first item in blocked queue.
int queryDisk()
{
	if ( numIORequests > 0) 									// If there are I/O requests
	{
		int targetSector = Disk_0Queue[0].diskSector;			// Use the first disk seek request's sector for comparison
		int difference = calcSectorSeek(targetSector, true);	// call calcSectorSeek, where difference can be 0, as the disk head has not passed over sectors yet.
		if ( difference == 0 ) { 								// Additional checks required however when difference IS 0
			if ( Disk_0Queue[0].operationType == 'B' ) {		// Check if this is a recently created write request
				difference += numDiskSectors;					// If yes then set difference to numDiskSectors (Representing one complete rotation of the disk)
				Disk_0Queue[0].operationType = 'V';				// Set the write request to validate (V) after this query completes
																// Possible error may occur if findNextEvent() returns an event with a time of 0 
																// (i.e. admit, exit, blocked) and the clock does not advance.
			}
		}
		return  (timeperSector * difference);					// Multiply sector delta by time per sector to get the time delta for the next I/O ready message. 
	}
	else return INT_MAX;										// No I/O requests = return INT_MAX
}

// Queries disk blocked queue for information relating to the first IO request in the queue (Process #/IO Request # it belongs to)
int * getNextIORequest()
{
	static int result[2]; 
	result[0] = Disk_0Queue[0].processID;
	result[1] = Disk_0Queue[0].IOreqID;
	return result;
}

// Returns difference between current sector and the target sector.
// Boolean condition provided - FALSE for when difference is NOT allowed to be 0 (Not during disk seek request creation for example)  
int calcSectorSeek(int targetSector, bool zero)
{
	int difference = targetSector - currSector;					// Init delta variable as simple arithmetic substraction first
	if (zero == false) {								// If difference IS NOT allowed to be zero
		if (difference <= 0) {									// If difference is negative OR zero
			difference += numDiskSectors;							// Add number of sectors to account for wrap around
		}
	}
	else {												// If difference IS allowed to be zero
		if (difference < 0) {									// If difference is negative
			difference += numDiskSectors;							// Add number of sectors to account for wrap around
		}
	}
	return difference;
}