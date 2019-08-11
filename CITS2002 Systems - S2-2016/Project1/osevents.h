/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
//#include <assert.h>

// DECLARE GLOBAL PREPROCESSOR CONSTANTS
#define INTERVAL 1
#define MIN_TO_SEC_FACTOR 60
#define SEC_TO_MICS_FACTOR 1000000
#define MAX_PROCESS 50
#define MAX_IO_PER_PROCESS 100

// DECLARING GLOBAL STRUCTURES
//requestIO - Contains IO request data, i.e. type of IO request, disk sector, etc. 
typedef struct
{
	char IOType;	// R - Read, W - Write, V - Write Verification				<--- Post Processed Data
	bool dataReady;	// true = IO request complete,
	int diskSector; // LOCATION of data requested on the disk
	int reqAge;		// RELATIVE current age of IO request 						<--- Post Processed Data
	int reqTime;	// RELATIVE time from process execution until Request is admitted
} requestIO;
//process - Contains process data, i.e. time to creation, time left before exiting, etc. 
typedef struct 
{
	char state;								// N - New, R - Ready/Running, B - Blocked, X - Exit
	int admitTime;							// ABSOLUTE time of admission 		<--- Post Processed Data
	int exitTime;							// ABSOLUTE time of termination 	<--- Post Processed Data
	int processLife;						// RELATIVE maximum life of process 
	int processAge;							// RELATIVE current age of process
	int numIO;								// TOTAL NUMBER of IO requests
	int currIO;								// CURRENT highest IO request processed
	requestIO reqIO[MAX_IO_PER_PROCESS];	// ARRAY of IO Requests
} process;
//processList - List of processes
typedef struct
{
	int numProcess;							// INDEX value for length of the process array
	process proc[MAX_PROCESS];				// ARRAY of process data
}processList;

// DECLARING EXTERN FUNCTIONS
int getDiskRPM();
int getNumDiskSectors();
int getTimeQuantum();
void initDisk();
void runOS(processList *);
void calculateOStime(processList *);
int * getNextIORequest();
int queryDisk();
void newDiskSeek(int, int, int, char);
void removeDiskSeek();



