/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"
// FILE SCOPE VARIABLES
static int init_timeQuantum;
static int init_diskRPM;
static int init_numDiskSectors;

// PREDECLARING FUNCTIONS 
void initOSEvents(char *);
void readScenario(char *, processList *);
void processLine (char *, int, processList *);
//---------------------------------------------------------------------------------------------------------------------------------//

// Initiates program 
// 1) First reads the scenario from defined file and parses data
// 2) Then initiates the parameters for the disk
// 3) Calls for OS simulation to begin
// 4) Calls for posts processing
void initOSEvents(char scenPath[])
{
	processList pList, *pList_Ptr = &pList;			// Set processList pointer pList_Ptr to processList pList - This will be used to store process + instruction data
	readScenario(scenPath, pList_Ptr);				// call function readScenario (init.c) to handle translation text file -> variables for program
	initDisk();										// Initialise disk file scope variables - time per sector, setting diskhead to sector 0, etc. (disk.c)
	runOS(pList_Ptr);								// Run the OS simulation (os.c)
	calculateOStime(pList_Ptr);						// Run the post simulation calculations (os.c)
}

// Reads the file and initialises the scenario - starting with the file scope variables (timeQuantum, diskRPM, numdiskSectors)
// and then interprets the instructions line by line (using processLine)
void readScenario(char scenPath[], processList *pList)
{
	FILE *scen;																		
	printf("=============================================================================\n");
	
	scen = fopen(scenPath, "r");															// Open scenario file for read-only access
	if (scen == NULL)																		// Validate scen, exit if source file cannot be opened.				
	{
		printf("Cannot open scenareqIO file '%s'\n", scenPath);
		exit(EXIT_FAILURE);
	}
	
	char line[BUFSIZ];																		// Sets buffer size to max
	int numLine = 0;																		// Index of number of lines processed so far
	while (fgets(line, sizeof line, scen) != NULL) 											// Call C library function fgets() <stdlib.h> to process the file line by line - Loop over this block until end of file
	{
		processLine(line, numLine, pList);													// Calls processLine() function to process current line
		numLine++;																			// Increase index 
	}
	fclose(scen);																			// Close the file
}

void setTimeQuantum(int n) { init_timeQuantum = n; }										// Accessor/Set functions for the static file scope variables
int getTimeQuantum() { return init_timeQuantum; }
void setDiskRPM(int n) { init_diskRPM = n; }
int getDiskRPM() { return init_diskRPM; }
void setNumDiskSectors(int n) { init_numDiskSectors = n; }
int getNumDiskSectors() { return init_numDiskSectors; }

//  Processes each line by calling C Library function strtok() <string.h>, iterating through tokens (columns) and interpreting this data.
//  Array of process structures is created line by line, containing instructions relevant to each respective process.
void processLine (char line[], int numLine, processList *pList)
{
	char *col_Ptr;
	col_Ptr = strtok(line, " \t\r\n"); 													// Set pointer token to the first token (aka column)
																						// This is deliminated by " ", "\t", "\r", "\n"  
	if (numLine <=2 && col_Ptr != NULL) {												// As long as we are in the first three lines and the pointer is not empty 	
		if (strcmp(col_Ptr,"timequantum") == 0) 										// IF the first column on the line is "timequantum"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column
			setTimeQuantum( atoi(col_Ptr) );												// SET this as init_timeQuantum (convert to int)
			printf("Quantum SET to: %i\n", init_timeQuantum);
		}
		else if (strcmp(col_Ptr,"diskrpm") == 0) 										// IF the first column on the line is "diskRPM"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column			
			setDiskRPM( atoi(col_Ptr) );													// SET this as init_diskRPM (convert to int)
			printf("DiskRPM SET to: %i\n", init_diskRPM);									
		} 
		else if (strcmp(col_Ptr,"disksectors") == 0) 									// IF the first column on the line is "disksectors"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column
			setNumDiskSectors( atoi(col_Ptr) );												// SET this as init_numDiskSectors (convert to int)
			printf("Disk Sectors SET to: %i\n", init_numDiskSectors);
		}
	} 
	else 
	{
		int timeData;																		// Holding variable for time data
		int PID;																			// Holding variable for process ID
		char eventType[6];																	// Holding variable for event type
		int IOData = NULL;																	// Holding variable for I/O request data
		
		timeData = atoi(col_Ptr);															// Records 1st token as time		
		col_Ptr = strtok(NULL, " \t\r\n"); 														// Advance token to the 2nd column
		PID = atoi(col_Ptr);																// Records 2nd token as process ID
		col_Ptr = strtok(NULL, " \t\r\n");   													// Advance token to the 3rd column
		strcpy(eventType, col_Ptr);														// Records 3rd token as event type
		if (strcmp(eventType,"read") == 0 || strcmp(eventType,"write") == 0) 			// IF this is a read/write event (otherwise DO NOTHING)
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance token to the 4th column
			IOData = atoi(col_Ptr);														// Records 4th token as I/O request data
		}
		printf("%i	%i	%s	%i\n", timeData, PID, eventType, IOData);
		
		if (strcmp(eventType,"admit") == 0) 											// IF event type is admit (for this line)
		{
			pList->proc[PID].admitTime = timeData;										// Process PID's creation time = time data 
			pList->proc[PID].state = 'N';												// Set the process's state to N - New
			pList->numProcess++;																// Increase process count
		}
		else if (strcmp(eventType,"exit") == 0) 										// ELSE IF event type is exit (for this line)
		{
			pList->proc[PID].processLife = timeData;									// Process PID's total life = time data 
		}
		else if (strcmp(eventType,"read") == 0) 										// ELSE IF event type is read (for this line)
		{
			int i = 0;																// set I/O request ID to 0
			while (pList->proc[PID].reqIO[i].IOType) { i++; }						// If this I/O request ID has already been initialised, increase the index until null is found
			pList->proc[PID].reqIO[i].IOType = 'R';									// SET this I/O request to Read (R)
			pList->proc[PID].numIO++;													// Increase internal I/O request count within structure array processList
			pList->proc[PID].reqIO[i].diskSector = IOData;							// SET the disksector for I/O request using IOData
			pList->proc[PID].reqIO[i].reqTime = timeData;							// SET the time until execution for I/O request using time data
		}
		else if (strcmp(eventType,"write") == 0) 										// ELSE IF event type is write (for this line)
		{
			int i = 0;
			while (pList->proc[PID].reqIO[i].IOType) { i++; }
			pList->proc[PID].reqIO[i].IOType = 'W';									// Same as above except set this I/O request to Write (W)
			pList->proc[PID].numIO++;
			pList->proc[PID].reqIO[i].diskSector = IOData;
			pList->proc[PID].reqIO[i].reqTime = timeData;
		}
	}
}

//main() functin - Checks that command line arguments are correct, terminating if they are not, else initiates initOSEvents();
int main(int argc, char *argv[]) 
{
	if (argc != 2) {																	// Check for incorrect usage of command line variables 
		fprintf(stderr, "Usage: %s [scenario]\n", argv[0]);
		exit(EXIT_FAILURE);
	} 
	else 																				// If no abnormalies found then execute program
	{
		initOSEvents(argv[1]);
		exit(EXIT_SUCCESS);
	}
	return 0;
}