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

// DECLARE GLOBAL PREPROCESSOR CONSTANTS
#define INTERVAL 1
#define MIN_TO_SEC_FACTOR 60
#define SEC_TO_MICS_FACTOR 1000000
#define MAX_PROCESS 50
#define MAX_IO_PER_PROCESS 100

// DECLARING GLOBAL STRUCTURES
typedef struct
{
	char IOType;	// R - Read, W - Write, V - Write Verification
	bool dataReady;	// true = IO request complete, 
	int diskSector, reqAge, reqTime;
} requestIO;

typedef struct 
{
	char state;
	int admitTime, exitTime, processLife, processAge, numIO, currIO;
	requestIO reqIO[MAX_IO_PER_PROCESS];
} process;

typedef struct
{
	int numProcess;
	process proc[MAX_PROCESS];
} processList;

typedef struct
{
	int processID, IOreqID, diskSector;
	char operationType;
} blockedQueue;

typedef struct
{
	int currSector, numIOrequests, timeperSector;
	blockedQueue diskQueue[MAX_PROCESS * MAX_IO_PER_PROCESS];
} disk;

// DECLARING GLOBAL VARIABLES
extern int g_timeQuantum, g_diskRPM, g_numDiskSectors;

// DECLARING GLOBAL FUNCTIONS
void initDisk(disk *);
void runOS(processList *, disk *);
void calculateOStime(processList *);
int * queryDisk_infoRecall(disk *);
int queryDisk_nextSector(disk *);
void newDiskSeek(disk *, int, int, int, char);
void removeDiskSeek(disk *);

