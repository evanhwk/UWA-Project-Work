/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

// DECLARING GLOBAL_VARs
int g_timeQuantum; 								// Max time for one process execution 
int g_diskRPM;									// Disk rotation speed 
int g_numDiskSectors; 							// Total number of Disk Sectors