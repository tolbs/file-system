/**************************************************************
* Class:  CSC-415-0# FALL 2020
* Name: 
* Student ID: 
* Github Id: 
* Project: 
*
* File: b_io.c
*
* Description: 
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"

#define MAXFCBS 20
uint64_t bufSize;

typedef struct b_fcb
{
	int linuxFd;	//holds the systems file descriptor
	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
	{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
		{
		fcbArray[i].linuxFd = -1; //indicates a free fcbArray
		}
		
	startup = 1;
	}

//Method to get a free FCB element
int b_getFCB ()
	{
	for (int i = 0; i < MAXFCBS; i++)
		{
		if (fcbArray[i].linuxFd == -1)
			{
			fcbArray[i].linuxFd = -2; // used but not assigned
			return i;		//Not thread safe
			}
		}
	return (-1);  //all in use
	}
	
// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
int b_open (char * filename, int flags)
	{
	int fd;
	int returnFd;
	
	
	//
	//
	
	
	if (startup == 0) b_init();  //Initialize our system
	
	// lets try to open the file before I do too much other work
	
	fd = open (filename, flags);
	if (fd  == -1)
		return (-1);		//error opening filename
		
	//Should have a mutex here
	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's
	fcbArray[returnFd].linuxFd = fd;	// Save the linux file descriptor
	//	release mutex
	
	//allocate our buffer
	fcbArray[returnFd].buf = malloc (BUFSIZE);
	if (fcbArray[returnFd].buf  == NULL)
		{
		// very bad, we can not allocate our buffer
		close (fd);							// close linux file
		fcbArray[returnFd].linuxFd = -1; 	//Free FCB
		return -1;
		}
		
	fcbArray[returnFd].buflen = 0; 			// have not read anything yet
	fcbArray[returnFd].index = 0;			// have not read anything yet
	return (returnFd);						// all set
	}




// Interface to write a buffer	
int b_write (int fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	
	
	int providedCount=count;	//	making a copy of count(no. of bytes in the buffer, provided)
	int returnValue=-1;			//	initializing the returnValue as -1
	
	//	if buffer length of the file fd , is less than the required buffer size (BUFSIZE or 512 bytes)
	//	than do the following , else write the buffer onto the file directly
	if(fcbArray[fd].buflen<BUFSIZE){
		
		//Getting the difference of the required and existing buffer length
		int difference=BUFSIZE-fcbArray[fd].buflen;
		
		//This variable records the previous difference , to identify the index , where to append
		//to the file fd's buffer
		int prevDifference=0;
		
		//This loop goes on until the count becomes 0 , or the provided buffer has been read fully
		while(count!=0)
		{
			//If the provided buffer length is less than the difference(512-existingbufferlength)
			//than copies the provided buffer length as it is and make the count 0 , to stop
			//the loop at next iteration , else copies the difference no. of bytes from 
			//provided buffer and decrement the count by the difference
			if(count<difference){
				difference=count;
				count=0;
			}
			else{
				count-=difference;
			}
			
			//appends the difference no. of bytes from provided buffer into the file buffer
			memcpy(fcbArray[fd].buf+fcbArray[fd].buflen,buffer+prevDifference,difference);
			
			//update the length of the file buffer , by the difference no. of bytes
			fcbArray[fd].buflen+=difference;
			
			
			if(fcbArray[fd].buflen>=BUFSIZE){
				
				//writing BUFSIZE no. of bytes from the buffer onto the file
				returnValue=write(fcbArray[fd].linuxFd,fcbArray[fd].buf,BUFSIZE);
				
				//make the current difference assign to previous difference to be used as index
				//to append in the next iteration
				prevDifference=difference;
				
				//make the difference equals to the BUFSIZE , denoting that the buffer is empty
				difference=BUFSIZE;
				
				//make the buffer empty
				memcpy(fcbArray[fd].buf,"",1);
				
				//make the length of the file buffer to 0
				fcbArray[fd].buflen=0;
			}
			// if the provided buffer has not been ended with '\n' , than it means it is the last
			// chunk read from the file , so write it to the file even it is not having BUFSIZE no. 
			// of bytes in it
			else if(buffer[providedCount-1]!='\n'){
				//Writing the last chunk onto the file , even it has not exactly BUFSIZE 
				//no. of bytes in it.
				returnValue=write(fcbArray[fd].linuxFd,fcbArray[fd].buf,fcbArray[fd].buflen);
			}
		}
	}
	else{
		//Writing the file fd's buffer onto the file , if it has exactly BUFSIZE no. of bytes
		//in it.
		returnValue=write(fcbArray[fd].linuxFd,fcbArray[fd].buf,BUFSIZE);		
	}
	//Returns the no. of bytes , actually written to file.
	return returnValue;
}
// Interface to read a buffer	
int b_read (int fd, char * buffer, int count)
	{
	int bytesRead;				// for our reads
	int bytesReturned;			// what we will return
	int part1, part2, part3;	// holds the three potential copy lengths
	
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	
	// number of bytes available to copy from buffer
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;	
	part3 = 0;				//only used if count > BUFSIZE
	if (remain >= count)  	//we have enough in buffer
		{
		part1 = count;		// completely buffered
		part2 = 0;
		}
	else
		{
		part1 = remain;				//spanning buffer (or first read)
		part2 = count - remain;
		}
				
	if (part1 > 0)	// memcpy part 1
		{
		memcpy (buffer, fcbArray[fd].buf + fcbArray[fd].index, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
		}
		
	if (part2 > 0)		//We need to read to copy more bytes to user
		{
		// Handle special case where user is asking for more than a buffer worth
		if (part2 > BUFSIZE)
			{
			int blocks = part2 / BUFSIZE; // calculate number of blocks they want
			bytesRead = read (fcbArray[fd].linuxFd, buffer+part1, blocks*BUFSIZE);
			part3 = bytesRead;
			part2 = part2 - part3;  //part 2 is now < BUFSIZE, or file is exusted
			}				
		
		//try to read BUFSIZE bytes into our buffer
		bytesRead = read (fcbArray[fd].linuxFd, fcbArray[fd].buf, BUFSIZE);
		
		// error handling here...  if read fails
		
		fcbArray[fd].index = 0;
		fcbArray[fd].buflen = bytesRead; //how many bytes are actually in buffer
		
		if (bytesRead < part2) // not even enough left to satisfy read
			part2 = bytesRead;
			
		if (part2 > 0)	// memcpy bytesRead
			{
			memcpy (buffer+part1+part3, fcbArray[fd].buf + fcbArray[fd].index, part2);
			fcbArray[fd].index = fcbArray[fd].index + part2;
			}
			
		}
	bytesReturned = part1 + part2 + part3;
	return (bytesReturned);	
	}
	
// Interface to Close the file	
void b_close (int fd)
	{
	close (fcbArray[fd].linuxFd);		// close the linux file handle
	free (fcbArray[fd].buf);			// free the associated buffer
	fcbArray[fd].buf = NULL;			// Safety First
	fcbArray[fd].linuxFd = -1;			// return this FCB to list of available FCB's 
	}
