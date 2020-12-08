/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: (KAAT) Kimberly Nivon, Angela Avina Garcia, 
*		Abishek Neralla, Tolby Lam
* Student ID: 916563133(Kimberly), 917579109(Angela), 
*		917725840(Abishek), 920787020(Tolby)
* Project: File System Project
*
* File: FileSystem.c
*
* Description: driver program before we use fsshell.c
*
**************************************************************/

#include "FileSystem.h"


INODE dir[DIR_SIZE] = {0};
METADATA storageBlocks[65536];
BITVECTOR bVector;
SYSFILETABLE systemTable;
PROCFILETABLE procTable;


int main(int argc, char const *argv[])
{
	createFileSystem();
	char *testString = "I am a test text file. ";
	createFile("test.txt", strlen(testString), 600, "Tolby", testString);
	createFile("test2.txt", 5, 700, "Kimberly", "Hello");
	getDirectoryInfo();
	int pid = 123;
	int canOpen = openFile("test2.txt", pid);
	printf("This open should be 0 - %d\n", canOpen);
	char *fData = readFile("test2.txt");
	printf("File has been read: %s\n\n", fData);
	int write = writeFile("test2.txt", "UPDATED DATA TEST WRITE");
	char *newData = readFile("test2.txt");
	printf("After write test: %s\n\n", newData);
	closeFile("test2.txt");
	int deleted = deleteFile("test.txt"); //- 0 RETURN IS GOOD! 1 = BAD
	printf("Deleted = %d\n\n", deleted); 
	char *fileThreeContent = "I am a third file to be put in memory";
	int accessMode = 777;
	createFile("test3.txt", strlen(fileThreeContent), 777, "Angela", fileThreeContent);
	createFile("test3.txt", strlen(fileThreeContent), 777, "Abishek", testString);

	printf("\n\n[UPDATED DIRECTORY]\n\n");
	getDirectoryInfo();
	deleted = deleteFile("test2.txt");
	printf("Deleted = %d\n", deleted);
	printf("%d\n", strlen(testString));
	printf("\n\n[UPDATED DIRECTORY]\n\n");
	getDirectoryInfo();
	return 0;
}


/**
 * TODO: make compatible with given partition creator
 * 
 * Our volume is actually an array for now, so we can work on other functions
 * until we manage to finish our b_io open/read/write
 **/
void createFileSystem()
{
	int i;
	for(i = 0; i < 10000; i++)
	{
		bVector.array[i] = 0;
	}
}


//
// Start Partition System
//
// This is the first function to call before your filesystem starts
// If the filename already exists, then the input values stored in 
// volSize and blockSize are ignored.  If the file does not exist, it will 
// be created to the specified volume size in units of the block size
// (must be power of 2) plus one for the partition header.
//
// On return 
// 		return value 0 = success;
//		return value -1 = file exists but can not open for write
//		return value -2 = insufficient space for the volume		
//		volSize will be filled with the volume size
//		blockSize will be filled with the block size

/*
int startPartitionSystem (char * filename, uint64_t * volSize, uint64_t * blockSize)
	{
	int fd;
	int retVal = PART_NOERROR;
	int accessRet = access(filename, F_OK);
	printf ("File %s does %sexist, errno = %d\n", filename, accessRet==-1?"not ":"",errno);
	
	accessRet = access(filename, R_OK | W_OK);
	printf ("File %s %sgood to go, errno = %d\n", filename, accessRet==-1?"not ":"",errno);
	
	//Some issue - 
	if (accessRet == -1)
		{
		if (errno == ENOENT)  //The volume file does not exist, initialize it
			{
			//File does not exist, create it and initialize
			fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if (fd == -1)
				{
				//Error
				return -1;
				}
				
			//insure that blocksize is a power of 2 (min 512);
			uint64_t blksz = *blockSize;
			printf("Block size is : %llu\n", (ull_t)blksz);
			if (blksz < MINBLOCKSIZE) //too small 
				blksz = MINBLOCKSIZE;
			
			// example 1000 0111 anded = 0 1010 1001 = 1000 when anded i.e. not power of 2	
			if ((blksz & (blksz - 1)) != 0) //not a power of 2
				{
				printf("%llu is not a power of 2\n", (ull_t)blksz);
				
				blksz = 1 << (uint64_t)(ceil(log2(blksz)));
				printf("Block size is now: %llu\n", (ull_t)blksz);
				*blockSize = blksz;
				} 
			// insure the volume size is a multiple of blockSize
			uint64_t blockCount = *volSize / blksz;
			*volSize = blockCount * blksz;
				
			int initRet = initializePartition (fd, *volSize, *blockSize);
			close (fd);
			}
		else
			{
			// Exists but can not read/write
			printf ("About to abort - problem opening file.  Error No: %d\n", errno);
			return -1; 
			}
		}
	
	// If there is no access issue or we fall through the if it is because we
	// have initialized the volume file already
	fd = open(filename, O_RDWR);
	partitionInfo_p buf = malloc (MINBLOCKSIZE);
	uint64_t readCount = read (fd, buf, MINBLOCKSIZE);
	if ((buf->signature == PART_SIGNATURE) && (buf->signature2 == PART_SIGNATURE2))
		{
		*volSize = buf->volumesize;
		*blockSize = buf->blocksize;
		partInfop = malloc (sizeof(partitionInfo_t)+strlen(buf->volumeName)+4);
		memcpy(partInfop, buf, sizeof(partitionInfo_t)+strlen(buf->volumeName)+4);
		partInfop->filename = malloc (strlen(filename)+4);
		strcpy(partInfop->filename, filename);
		partInfop->fd = fd;
		retVal = PART_NOERROR;
		}
	else
		{
		*volSize = 0;
		*blockSize = 0;
		retVal = PART_ERR_INVALID;
		}
		
	free (buf);
	if (retVal != PART_NOERROR)
		close (fd);
	return retVal;
	}
*/

void createFile(char *fileName, short fileSize, int fileMode, char *owner, char *data)
{
	long dirIndex = hash(fileName);
	/**
	 * TODO: need error handling here- maybe copy from fslow.c
	 **/

	//printf("HASHED %d\n", dirIndex); //INDEX OF DIRECTORY HASH - TESTING
	INODE dirEnt = dir[dirIndex];
	long memIndex = findFreeIndex();
	storageBlocks[memIndex] = createFileDesc(fileName, fileSize, fileMode, owner);
	long descIndex = memIndex;
	memIndex = findFreeIndex();
	storageBlocks[descIndex].node.fDescription.indexOfNode = memIndex;
	dirEnt.fileContent = storageBlocks[descIndex].node.fDescription;
	dirEnt.indexOfFile = descIndex;
	dir[dirIndex] = dirEnt;
	//printf("%s\n", storageBlocks[memIndex]->node->fDescription->fileName); //TEST FOR SEG FAULT - CURRENTLY WORKING
	
	
	if(fileSize > 254)
	{
		int numOfIndexNodes = fileSize / 254;
		int i;
		long iNodeIndex;
		int startIndex = 0;		
		
		iNodeIndex = memIndex;
		storageBlocks[memIndex] = createFileIndex();	
		for(i = 0; i <= numOfIndexNodes; i++)
		{
			memIndex = findFreeIndex();
			storageBlocks[iNodeIndex].node.iNode.index[i] = memIndex;
			//printf("%d INDEXTEST\n", storageBlocks[iNodeIndex]->node->iNode->index[i]); - TEST FOR CORRECT INDEX ADDING TO INDEX NODE
			char substring[254];
			if(i == 0)
			{
				//strncpy(substring, data, 254);
				int j;
				for(j = 0; j < 254; j++)
				{
					substring[j] = data[j];
				}
				storageBlocks[memIndex] = createFileData(substring, 254);
			}
			else if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				storageBlocks[memIndex] = createFileData(substring, fileSize - startIndex);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				storageBlocks[memIndex] = createFileData(substring, 254);
			}
			startIndex += 254;
		}
	}
	else
	{
		storageBlocks[memIndex] = createFileData(data, fileSize);
	}
	//printf("%s\n", storageBlocks[memIndex]->node->fData->data); //TEST FOR SEG FAULT ON DATA NODE - CURRENTLY WORKING
	
}

METADATA createFileIndex()
{
	METADATA retVal;
	retVal.type = 2;
	MEM_NODE node;
	INDEX_NODE fIndex;
	node.iNode = fIndex;
	retVal.node = node;
	return retVal;
}

METADATA createFileData(char *data, int size)
{
	METADATA retVal;
	retVal.type = 3;
	MEM_NODE node;
	FILE_DATA fData;
	strncpy(fData.data, data, size);
	node.fData = fData;
	retVal.node = node;
	return retVal;
}

METADATA createFileDesc(char *fileName, short fileSize, int fileMode, char *owner)
{
	time_t now = time(0);
	METADATA retVal;
	retVal.type = 1;
	MEM_NODE node;
	FILEDESC description;
	strcpy(description.fileName, fileName);
	description.fileSize = fileSize;
	description.fileMode = fileMode;
	strcpy(description.owner, owner);
	//description.creationTime = now;
	node.fDescription = description;
	retVal.node = node;	
	return retVal;
}

int deleteFile(char *fileName)
{
	long index = hash(fileName);
	INODE dirEnt = dir[index];
	FILEDESC blankContent;
	strcpy(blankContent.fileName, "\0");
	if(strcmp(dirEnt.fileContent.fileName, fileName) == 0)
	{
		//DELETE THE FILE, CHECK FOR NEXT POINTER, CLEAR THE BITVECTOR BLOCK FOR EACH INDEX
		printf("\nDeleting file: %s\n", dirEnt.fileContent.fileName);
		if(dirEnt.next != NULL)
		{
			dir[index] = *dirEnt.next;
		}
		freeMemory(dirEnt.indexOfFile, dirEnt.fileContent.fileSize);
		dir[index].fileContent = blankContent;
		return 0;
	}
	else
	{
		INODE first = dirEnt;
		while(dirEnt.next != NULL)
		{
			dirEnt = *dirEnt.next;
			if(strcmp(dirEnt.fileContent.fileName, fileName) == 0)
			{
				printf("Deleting file: %s\n", dirEnt.fileContent.fileName);
				if(dirEnt.next != NULL)
				{
					INODE current = first;
					while(strcmp(current.next->fileContent.fileName, fileName) != 0)
					{
						current = *current.next;
					}
					current.next->next = dirEnt.next;
				}
				dir[index] = first;
				freeMemory(dirEnt.indexOfFile, dirEnt.fileContent.fileSize);
				dir[index].fileContent = blankContent;
				//DELETE THE FILE, CHECK FOR NEXT POINTER, CLEAR THE BITVECTOR BLOCK FOR EACH INDEX
				return 0;
			}
		}
	}
	return 1;
}

int openFile(char *fileName, int pid)
{
	printf("Request opening file: %s\n", fileName);
	procTable.pid = pid;
	long dirIndex = hash(fileName);
	INODE dirEnt = dir[dirIndex];
	int haveFile = 1;
	while((haveFile = strcmp(dirEnt.fileContent.fileName, fileName) != 0) && (strlen(dirEnt.fileContent.fileName) > 0))
	{
		dirEnt = *dirEnt.next;
	}
	int isOpen = 0;
	int i;
	if(haveFile == 0) //means we have the file if the return is 0
	{
		for(i = 0; i < systemTable.numOpen; i++)
		{
			if(strcmp(systemTable.openFiles[i].fileName, fileName) == 0)
			{
				isOpen = 1;
				//CANNOT OPEN THE FILE BECAUSE IT IS ALREADY OPEN
				return 1;
			}
		}
		if(isOpen == 0)
		{
			printf("File being opened/added to table\n");
			systemTable.openFiles[systemTable.numOpen] = dirEnt.fileContent;
			systemTable.fdIndex[systemTable.numOpen] = dirEnt.indexOfFile;
			systemTable.dataIndex[systemTable.numOpen] = dirEnt.fileContent.indexOfNode;
			systemTable.numOpen++;
			strcpy(procTable.fileNames[procTable.numOpen], fileName);
			procTable.canWrite[procTable.numOpen] = checkCanWrite(dirEnt.fileContent.fileMode);
			procTable.numOpen++;
			return 0;
		}
	}
	else
	{
		printf("File %s does not exist\n", fileName);
		return 1;
	}
	return 1;
}

void closeFile(char *fileName)
{
	printf("Closing File: %s\n", fileName);
	long dirIndex = hash(fileName);
	INODE dirEnt = dir[dirIndex];
	int haveFile = 1;
	while((haveFile = strcmp(dirEnt.fileContent.fileName, fileName) != 0) && (strlen(dirEnt.fileContent.fileName) > 0))
	{
		dirEnt = *dirEnt.next;
	}
	int i;
	if(haveFile == 0) //means we have the file if the return is 0
	{
		for(i = 0; i < procTable.numOpen; i++)
		{
			if(strcmp(procTable.fileNames[i], fileName) == 0)
			{
				procTable.canWrite[i] = 0;
				shiftProcTable(i);
				int sysIndex = findInProcTable(fileName);
				systemTable.fdIndex[sysIndex] = -1;
				systemTable.dataIndex[sysIndex] = -1;
				shiftSysTable(sysIndex);	
			}
		}
	}
	else
	{
		printf("File %s does not exist\n", fileName);
	}
}

void shiftProcTable(int index)
{
	printf("Updating process table for process ID%d\n", procTable.pid);
	if(procTable.numOpen > 1)
	{
		strcpy(procTable.fileNames[index], procTable.fileNames[procTable.numOpen - 1]);
		strcat(procTable.fileNames[index], "\0");
		procTable.canWrite[index] = procTable.canWrite[procTable.numOpen - 1];
		procTable.numOpen--;
	}
}

void shiftSysTable(int index)
{
	printf("Updating system table\n");
	if(systemTable.numOpen > 1)
	{	
		systemTable.openFiles[index] = systemTable.openFiles[systemTable.numOpen - 1];
		systemTable.fdIndex[index] = systemTable.fdIndex[systemTable.numOpen - 1];
		systemTable.dataIndex[index] = systemTable.dataIndex[systemTable.numOpen - 1];
		systemTable.numOpen--;
	}
}

char *readFile(char *fileName)
{
	long index = findInProcTable(fileName);
	short fileSize = systemTable.openFiles[index].fileSize;
	if(index == -1)
	{
		printf("ERROR: File %s is not open\n", fileName);
		return "ERROR";
	}
	printf("Reading file %s\n", fileName);
	int numIndexNodes = systemTable.openFiles[index].fileSize / 254;
	char retVal[fileSize + 1];
	char *y;
	if(numIndexNodes == 0)
	{
		strncpy(retVal, storageBlocks[systemTable.dataIndex[index]].node.fData.data, systemTable.openFiles[index].fileSize);
		retVal[systemTable.openFiles[index].fileSize] = '\0';
		y = (char *) &retVal;
		return y;
	}
	int i;	
	for(i = 0; i <= numIndexNodes; i++)
	{
		strcat(retVal, storageBlocks[storageBlocks[systemTable.dataIndex[index]].node.iNode.index[i]].node.fData.data);
	}
	strcat(retVal, "\0");
	y = (char *) &retVal;
	return y;
}

int writeFile(char *fileName, char *data)
{
	long index = findInProcTable(fileName);
	short newFileSize = strlen(data);
	if(index == -1)
	{
		printf("ERROR: File %s is not open\n", fileName);
		return -1;
	}
	int i;
	int foundFile = 1; //1 is bad, not open
	int procTableIndex;
	for(i = 0; i < procTable.numOpen; i++)
	{
		if((foundFile = strcmp(procTable.fileNames[i], fileName)) == 0)
		{
			procTableIndex = i;
		}
	}
	if(foundFile == 1)
	{
		return -1;
	}
	if(procTable.canWrite[procTableIndex] != 1)
	{
		return -1;
	}
	short currentFileSize = systemTable.openFiles[index].fileSize;
	int numDataCurrent = currentFileSize / 254;
	int numDataNew = newFileSize / 254;
	if(numDataCurrent == numDataNew)
	{
		replaceSameSize(systemTable.openFiles[index].indexOfNode, data, numDataCurrent);
		storageBlocks[systemTable.fdIndex[index]].node.fDescription.fileSize = newFileSize;
		systemTable.openFiles[index].fileSize = newFileSize;
	}
	else if(numDataCurrent < numDataNew)
	{
		replaceSmallerCurrent(systemTable.fdIndex[index], systemTable.openFiles[index].indexOfNode, data, numDataNew, numDataCurrent);
		storageBlocks[systemTable.fdIndex[index]].node.fDescription.fileSize = newFileSize;
		systemTable.openFiles[index].fileSize = newFileSize;
	}
	else if(numDataCurrent > numDataNew)
	{
		replaceLargerCurrent(systemTable.fdIndex[index], systemTable.openFiles[index].indexOfNode, data, numDataNew, numDataCurrent);
		storageBlocks[systemTable.fdIndex[index]].node.fDescription.fileSize = newFileSize;
		systemTable.openFiles[index].fileSize = newFileSize;
	}
	return 1;
}

/**
 * Free-Space Management Stuff
 **/
void replaceSmallerCurrent(long fdIndex, long memIndex, char *data, int numDataNew, int numDataCurrent)
{
	if(storageBlocks[memIndex].type == 2)
	{
		int i;
		char substring[254];
		int startIndex = 0;
		short fileSize = strlen(data);
		for(i = 0; i <= numDataCurrent; i++)
		{
			
			int currDataIndex = storageBlocks[memIndex].node.iNode.index[i];
			storageBlocks[currDataIndex].node.fData.data[0] = '\0';
			if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			startIndex += 254;
		}

		long nextDataIndex;
		for(i = (numDataNew - numDataCurrent); i <= numDataNew; i++)
		{
			nextDataIndex = findFreeIndex();
			storageBlocks[memIndex].node.iNode.index[i] = nextDataIndex;
			//printf("%d INDEXTEST\n", storageBlocks[iNodeIndex]->node->iNode->index[i]); - TEST FOR CORRECT INDEX ADDING TO INDEX NODE
			char substring[254];
			if(i == 0)
			{
				//strncpy(substring, data, 254);
				int j;
				for(j = 0; j < 254; j++)
				{
					substring[j] = data[j];
				}
				storageBlocks[nextDataIndex] = createFileData(substring, 254);
			}
			else if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				storageBlocks[nextDataIndex] = createFileData(substring, fileSize - startIndex);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				storageBlocks[nextDataIndex] = createFileData(substring, 254);
			}
			startIndex += 254;
		}

	}
	else if(storageBlocks[memIndex].type == 3)
	{
		long iNodeIndex = findFreeIndex();
		METADATA indexNode = createFileIndex();
		int indexCounter = 0;
		indexNode.node.iNode.index[indexCounter] = memIndex;
		//overwrite first data node
		int startIndex = 0;
		char substring[254];
		int j;
		int count = 0;
		for(j = startIndex; j < 254; j++)
		{
			substring[count] = data[j];
			count++;
		}
		startIndex += 254;
		strcpy(storageBlocks[memIndex].node.fData.data, substring);
		indexCounter++;
		storageBlocks[fdIndex].node.fDescription.indexOfNode = iNodeIndex;

		int i;
		short fileSize = strlen(data);
		long nextDataIndex;
		for(i = 1; i <= numDataNew; i++)
		{
			nextDataIndex = findFreeIndex();
			storageBlocks[iNodeIndex].node.iNode.index[i] = nextDataIndex;
			//printf("%d INDEXTEST\n", storageBlocks[iNodeIndex]->node->iNode->index[i]); - TEST FOR CORRECT INDEX ADDING TO INDEX NODE
			char substring[254];
			if(i == 0)
			{
				//strncpy(substring, data, 254);
				int j;
				for(j = 0; j < 254; j++)
				{
					substring[j] = data[j];
				}
				storageBlocks[nextDataIndex] = createFileData(substring, 254);
			}
			else if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				storageBlocks[nextDataIndex] = createFileData(substring, fileSize - startIndex);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				storageBlocks[nextDataIndex] = createFileData(substring, 254);
			}
			startIndex += 254;
		}
	}
}

void replaceLargerCurrent(long fdIndex, long memIndex, char *data, int numDataNew, int numDataCurrent)
{
	short fileSize = strlen(data);
	if(fileSize > 254)
	{
		int extraData = numDataCurrent - numDataNew;
		int i;
		for(i = numDataCurrent; i > extraData; i--)
		{
			long dataIndex = storageBlocks[memIndex].node.iNode.index[i];
			storageBlocks[dataIndex].node.fData.data[0] = '\0';
			storageBlocks[memIndex].node.iNode.index[i] = 0;
			freeBitVector(dataIndex);
		}
		char substring[254];
		int startIndex = 0;
		for(i = 0; i <= numDataNew; i++)
		{			
			int currDataIndex = storageBlocks[memIndex].node.iNode.index[i];
			storageBlocks[currDataIndex].node.fData.data[0] = '\0';
			if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			startIndex += 254;
		}
	}
	else if(fileSize <= 254)
	{
		int i;
		for(i = 0; i <= numDataCurrent; i++)
		{
			long dataIndex = storageBlocks[memIndex].node.iNode.index[i];
			storageBlocks[memIndex].node.iNode.index[i] = 0;
			freeBitVector(dataIndex);
		}
		storageBlocks[memIndex].type = 3;
		char substring[254];
		for(i = 0; i < fileSize; i++)
		{
			substring[i] = data[i];
		}
		strcat(substring, "\0");
		strcpy(storageBlocks[memIndex].node.fData.data, substring);
	}
}

void replaceSameSize(long memIndex, char *data, int numData) 
{
	if(storageBlocks[memIndex].type == 2) //if Index Node
	{
		int i;
		char substring[254];
		int startIndex = 0;
		short fileSize = strlen(data);
		for(i = 0; i <= numData; i++)
		{
			
			int currDataIndex = storageBlocks[memIndex].node.iNode.index[i];
			storageBlocks[currDataIndex].node.fData.data[0] = '\0';
			if((fileSize - startIndex) <= 254)
			{
				int j;
				int count = 0;
				for(j = startIndex; j < fileSize; j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			else
			{
				int j;
				int count = 0;
				for(j = startIndex; j < (startIndex + 254); j++)
				{
					substring[count] = data[j];
					count++;
				}
				strcat(substring, "\0");
				strcpy(storageBlocks[currDataIndex].node.fData.data, substring);
			}
			startIndex += 254;
		}	
	}
	else if(storageBlocks[memIndex].type == 3)
	{
		storageBlocks[memIndex].node.fData.data[0] = '\0';
		strncpy(storageBlocks[memIndex].node.fData.data, data, strlen(data));
	}
}

/**
 * Finds the file in the process table and returns its index
 **/
long findInProcTable(char *fileName)
{
	int i;
	int foundFile = 1; //1 IS BAD, 0 means found! (strcmp return -_-)
	for(i = 0; i < procTable.numOpen; i++)
	{
		if((foundFile = strcmp(procTable.fileNames[i], fileName)) == 0)
		{
			return findInSystemTable(fileName);
		}
	}
	if(foundFile == 0)
	{
		return findInSystemTable(fileName);
	}
	return -1;
}

/**
 * Finds file in the system table and returns its index
 **/
long findInSystemTable(char *fileName)
{
	int i;
	int foundFile = 1;
	for(i = 0; i < systemTable.numOpen; i++)
	{
		if((foundFile = strcmp(systemTable.openFiles[i].fileName, fileName)) == 0)
		{
			return i;
		}
	}
	return -1;
}

//this doesn't make sense? vvvv
int checkCanWrite(int fileMode)
{
	//switch the fileMode. Check if you can write to it. return 1 for yes, 0 for no.
	return 1;
}

void freeMemory(long indexOfFileDesc, short fileSize)
{
	METADATA toDelete = storageBlocks[indexOfFileDesc];
	printf("Removing memory block at index: %ld\n", indexOfFileDesc);
	if(toDelete.type == 1)
	{
		printf("Removing file descriptor\n");
		freeMemory(toDelete.node.fDescription.indexOfNode, fileSize);
		toDelete.type = 0;
		toDelete.node.fDescription.fileName[0] = '\0';
		freeBitVector(indexOfFileDesc);

		/**
		 * TODO: need to free bit vector here after function is implemented
		 */
	}
	else if(toDelete.type == 2)
	{
		printf("Removing index node\n");
		int i;
		for(i = 0; i <= (fileSize / 254); i++)
		{
			freeBitVector(indexOfFileDesc);
			freeMemory(toDelete.node.iNode.index[i], fileSize);
			toDelete.type = 0;

			/**
		 	* TODO: need to free bit vector here after function is implemented
		 	*/
		}		
	}

	else if(toDelete.type == 3)
	{
		printf("Removing file data\n");
		toDelete.type = 0;
		toDelete.node.fData.data[0] = '\0';
		freeBitVector(indexOfFileDesc);
	}
}

void freeBitVector(long index)
{
	int byteIndex = index / 8;
	int offset = index % 8;
	bVector.array[byteIndex] &= ~(1 << offset);
	//printf("Freed bitVector for memory position: %d\n", byteIndex + offset);
}

void getDirectoryInfo()
{
	long i;
	for(i = 0; i < DIR_SIZE; i++)
	{
		if(strlen(dir[i].fileContent.fileName) > 0)
		{
			printf("***FILE***\n");
			getFileInfo(&dir[i]);
			printf("\n");
		}
	}
}

void getFileInfo(INODE *dirEnt)
{
	printf("File Name: %s\n", dirEnt->fileContent.fileName);
	printf("File Size: %d\n", dirEnt->fileContent.fileSize);
	printf("Access Mode: %d\n", dirEnt->fileContent.fileMode);
	printf("Owner: %s\n", dirEnt->fileContent.owner);
	printf("Index of file descriptor in memory: %ld\n", dirEnt->indexOfFile);
	printf("Index of first iNode/Data: %ld\n", dirEnt->fileContent.indexOfNode);
}


//FINDFREEINDEX NEEDS TO BE THE BITVECTOR CHECKING
long findFreeIndex()
{
	//ALL THIS FUNCTION BAD
	//NEED TO LEARN BITVECTOR DUDE
	int i;
	for(i = 0; i < 8192; i++)
	{
		if(bVector.array[i] == 0)
		{
			bVector.array[i] |= 1;
			return i * 8;
		}
		else
		{
			int j;
			for(j = 1; j < 8; j++)
			{
				if ((bVector.array[i] & (1 << j)) == 0)
				{
					bVector.array[i] |= (1 << j);
					return (i * 8) + j;
				}
			}
		}
	}
	return -1;
}

//found hash function online...
unsigned long hash(const char *s)
{
    unsigned long h;
    unsigned const char *us;
   
    /* cast s to unsigned const char * */
    /* this ensures that elements of s will be treated as having values >= 0 */
    us = (unsigned const char *) s;
   
    h = 0;
    while(*us != '\0') {
        h = (h * 256 + *us) % DIR_SIZE;
        us++;
    } 
    
    return h;
}