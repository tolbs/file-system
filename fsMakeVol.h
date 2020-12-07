/**************************************************************
* Class:  CSC-415
* Name: Team Penta - Duy Nguyen, Taylor Artunian, Wameedh Mohammed Ali
* Student ID: 917446249 (Duy), 920351715 (Taylor), 920678405 (Wameedh)
* Project: Basic File System - PentaFS
*
* File: fsMakeVol.h
*
* Description: This is the header file that holds the set of routines used to create a volume
*              for the Penta File System.
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "fsLow.h"
#include "bitMap.h"
#include "mfs.h"

#define DATA_BLOCKS_PER_INODE	4	//Number of data blocks to allocate per on inode.
#define VCB_START_BLOCK		0

/* Struct for the VCB of the disk. Use allocateVCB to allocate
 * the proper amount of memory.
 */

#ifndef _MAKEVOL
#define _MAKEVOL

typedef struct { //structure to hold all the data of our VCB for ease of access
  char header[16];
  uint64_t volumeSize;
  uint64_t blockSize;
  uint64_t diskSizeBlocks;
  uint64_t vcbStartBlock;
  uint64_t totalVCBBlocks;
  uint64_t inodeStartBlock;
  uint64_t totalInodes;
  uint64_t totalInodeBlocks;
  //struct mfs_dirent rootDir;
  uint64_t freeMapSize;
  uint32_t freeMap[];
} mfs_VCB;
#endif

/* Utility function for rounding up integer division. */
uint64_t ceilDiv(uint64_t, uint64_t);

/* Allocates space for an mfs_VCB with size as a multiple of block size and
 * assigns it to the pointer argument. Returns number of blocks allocated. */
int allocateVCB(mfs_VCB**);

/* Reads blocks from the disk. */
uint64_t fsRead(void*, uint64_t, uint64_t);

/* Writes blocks to the disk, edits the freeMap and writes to
 * disk.
 */
uint64_t fsWrite(void*, uint64_t, uint64_t);

/* Frees blocks in the freeMap and writes to disk. */
void fsFree(void*, uint64_t, uint64_t);

/* Checks if there is enough contiguous blocks for a requested number of blocks
return 0 for free 1 for full */
int checkIfStorageIsAvalibale(int numberOfRequestedBlocks);

/* Return the first free block */

uint64_t getFreeBlock();

/* Reads the VCB into memory. */
uint64_t readVCB();

/* Writes the VCB to disk. */
uint64_t writeVCB();

/* Gets pointer to the in memory VCB. */
mfs_VCB* getVCB();

/* Utility function for printing the VCB in hex and ASCII. */
void printVCB();

/* Creates a new volume with the specified fileName, volumeSize and blockSize.
 * Initializes VCB and inodes.
 * Return Values:  0 Volume created successfully.
 *                -1 File exists but cannot be written to.
 *                -2 Insufficient space to create volume.
 *                -3 Volume already exists.
 *                -4 System not initialized. (Call init first).
 */
int createVolume(char*, uint64_t, uint64_t);

/* Tries to open a volume. */

void openVolume(char*);

void closeVolume();
