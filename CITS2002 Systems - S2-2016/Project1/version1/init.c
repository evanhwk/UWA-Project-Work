/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// PREDECLARING FUNCTIONS 
void initScenario(char[]);
void processLine (char[], int);


// Reads the file and initialises the scenario - starting with the global variables (timeQuantum, diskRPM, numdiskSectors)
// and then interprets the instructions line by line (using processLine)
void initScenario(char scenPath[])
{
	FILE *scen;
	char line[BUFSIZ];																		// Sets buffer size to max
	int numLine = 0;																		// Index - Number of lines processed so far
	printf("=============================================================================\n");
	
	scen = fopen(scenPath, "r");															// Open scenario file for read-only access
	if (scen == NULL)																		// Validate scen, exit if source file cannot be opened.				
	{
		printf("Cannot open scenareqIO file '%s'\n", scenPath);
		exit(EXIT_FAILURE);
	}
	
	while (fgets(line, sizeof line, scen) != NULL) 											// Call C library function fgets() <stdlib.h> to process the file line by line - Loop over this block until end of file
	{
		processLine(line, numLine);															// Calls processLine() function to process current line
		numLine++;																			// Increase index
	}
	fclose(scen);																			// Close the file
}

//  Processes each line by calling C Library function strtok() <string.h>, iterating through tokens (columns) and interpreting this data.
//  Global array of process structures is created line by line, containing instructions relevant to each respective process.
void processLine (char line[], int numLine)
{
	char *col_Ptr;
	col_Ptr = strtok(line, " \t\r\n"); 													// Set pointer token to the first token (aka column)
																						// This is deliminated by " ", "\t", "\r", "\n"  
	if (numLine <=2 && col_Ptr != NULL) {												// As long as we are in the first three lines and the pointer is not empty 	
		if (strcmp(col_Ptr,"timequantum") == 0) 											// IF the first column on the line is "timequantum"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column
			g_timeQuantum = atoi(col_Ptr);													// SET this as global variable timeQuantum (convert to int)
			printf("Quantum SET to: %i\n", g_timeQuantum);
		}
		else if (strcmp(col_Ptr,"diskrpm") == 0) 										// IF the first column on the line is "diskRPM"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column
			g_diskRPM = atoi(col_Ptr);														// SET this as global variable diskRPM (convert to int)
			printf("DiskRPM SET to: %i\n", g_diskRPM);									
		} 
		else if (strcmp(col_Ptr,"disksectors") == 0) 									// IF the first column on the line is "disksectors"
		{
			col_Ptr = strtok(NULL, " \t\r\n"); 												// Advance pointer token to the 2nd column
			g_numdiskSectors = atoi(col_Ptr); 												// SET this as global variable numDiskSectors (convert to int)
			printf("Disk Sectors SET to: %i\n", g_numdiskSectors);
		}
	} 
	else 
	{
		int timeData;																		// Holding variable for time data
		int PID;																			// Holding variable for process ID
		char eventType[6];																	// Holding variable for event type
		int IOData = NULL;																	// Holding variable for I/O request data
		
		timeData = atoi(col_Ptr);															// Records 1st token as time		
		col_Ptr = strtok(NULL, " \t\r\n"); 													// Advance token to the 2nd column
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
			processList[PID].admitTime = timeData;										// Process PID's creation time = time data 
			processList[PID].state = 'N';												// Set the process's state to N - New
			g_numProcess++;																// Increase global variable keeping track of process count - g_numProcess
		}
		else if (strcmp(eventType,"exit") == 0) 										// ELSE IF event type is exit (for this line)
		{
			processList[PID].processLife = timeData;									// Process PID's total life = time data 
		}
		else if (strcmp(eventType,"read") == 0) 										// ELSE IF event type is read (for this line)
		{
			int index = 0;																// set I/O request ID to 0
			while (processList[PID].reqIO[index].IOType) { index++; }					// If this I/O request ID has already been initialised, increase the index until null is found
			processList[PID].reqIO[index].IOType = 'R';									// SET this I/O request to Read (R)
			processList[PID].numIO++;													// Increase internal I/O request count within structure array processList
			processList[PID].reqIO[index].diskSector = IOData;							// SET the disksector for I/O request using IOData
			processList[PID].reqIO[index].reqTime = timeData;							// SET the time until execution for I/O request using time data
		}
		else if (strcmp(eventType,"write") == 0) 										// ELSE IF event type is write (for this line)
		{
			int index = 0;
			while (processList[PID].reqIO[index].IOType) { index++; }
			processList[PID].reqIO[index].IOType = 'W';									// Same as above except set this I/O request to Write (W)
			processList[PID].numIO++;
			processList[PID].reqIO[index].diskSector = IOData;
			processList[PID].reqIO[index].reqTime = timeData;
		}
	}
}

int main(int argc, char *argv[]) 
{
	if (argc != 2) {																	// Check for incorrect usage of command line variables 
		fprintf(stderr, "Usage: %s [scenareqIO]\n", argv[0]);
		exit(EXIT_FAILURE);
	} 
	else 																				// If no abnormalies found then execute program
	{
		initScenario(argv[1]);															// call function initScenario (init.c) to initialise file translation -> variables for program
		initDisk();																		// Initialise disk variables - calculating time per sector, setting diskhead to sector 0, etc. (disk.c)
		runOS();																		// Run the OS simulation (os.c)
		calculateOStime();
	}
	return 0;
}