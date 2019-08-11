/*
   CITS2002 Project 1 2016
   Name(s):		Evan Huang
   Student number(s):	20916873
   Date:		23/09/2016
*/
#include "osevents.h"

int g_numProcess;
process processList[MAX_PROCESS];										// Global array of process structures

int length = g_numProcess;
int sumProcessAge = 0;
for (int i = 0; i < length; i++)
{
	sumProcessAge += processList[i].processAge;
}

int sumRead = 0;
for (int i = 0; i < length; i++)
{
	for (int j = 0; j < length; j++)
	{
		if (processList[i].reqIO[j].IOType == 'R') {
			sumRead += processList[i].reqIO[j].reqAge;
		}
	}
}

int sumWrite = 0;
for (int i = 0; i < length; i++)
{
	for (int j = 0; j < length; j++)
	{
		if (processList[i].reqIO[j].IOType == 'V') {
			sumRead += processList[i].reqIO[j].reqAge;
		}
	}
}