#define DIR_SIZE 10

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "structs.h"

void createFileSystem();
void createFile(char *fileName, short fileSize, int fileMode, char *owner, char *data);
int deleteFile(char *fileName);
void getDirectoryInfo();
long findFreeIndex();
void freeMemory(long indexOfFileDesc, short fileSize);
void freeBitVector(long index);
void getFileInfo(INODE *dirEnt);
long findInProcTable(char *fileName);
long findInSystemTable(char *fileName);
int openFile(char *fileName, int pid);
int checkCanWrite(int fileMode);
int writeFile(char *fileName, char *data);
char *readFile(char *fileName);
void shiftSysTable(int index);
void shiftProcTable(int index);
void closeFile(char *fileName);
void replaceSameSize(long memIndex, char *data, int numData);
void replaceSmallerCurrent(long fdIndex, long memIndex, char *data, int numDataNew, int numDataCurrent);
void replaceLargerCurrent(long fdIndex, long memIndex, char *data, int numDataNew, int numDataCurrent);
METADATA createFileDesc(char *fileName, short fileSize, int fileMode, char *owner);
METADATA createFileData(char *data, int size);
METADATA createFileIndex();
unsigned long hash(const char *s);