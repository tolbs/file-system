/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: (KAAT) Kimberly Nivon, Angela Avina Garcia, 
*		Abishek Neralla, Tolby Lam
* Student ID: 916563133(Kimberly), 917579109(Angela), 
*		917725840(Abishek), 920787020(Tolby)
* Project: File System Project
*
* File: structs.h
*
* Description:	holds all the structs used in KAAT FS
*
**************************************************************/


typedef struct FILEDESC
{
	char fileName[40];
	short fileSize;
	int fileMode;
	char owner[40];
	long indexOfNode;
} FILEDESC;

typedef struct INDEX_NODE
{
	short index[127];
} INDEX_NODE;

typedef struct FILE_DATA
{
	char data[254];
} FILE_DATA;

typedef union MEM_NODE
{
	struct FILEDESC fDescription;
	struct INDEX_NODE iNode;
	struct FILE_DATA fData;
} MEM_NODE;

typedef struct METADATA
{
	short type;
	union MEM_NODE node;
} METADATA;

typedef struct INODE
{
	FILEDESC fileContent;
	long indexOfFile;
	struct INODE *next;
} INODE;

typedef struct directory
{
	struct INODE *hashTable[DIR_SIZE];
} DIRECTORY;


//should just have a pointer to FD node

typedef struct BITVECTOR
{
	unsigned char array[8192];
} BITVECTOR;

typedef struct SYSFILETABLE
{
	long fdIndex[DIR_SIZE];
	FILEDESC openFiles[DIR_SIZE];
	long numOpen;
	long dataIndex[DIR_SIZE];
	//might need pointer to fd or fd info
} SYSFILETABLE;

typedef struct PROCFILETABLE
{
 	int pid;
 	char fileNames[DIR_SIZE][40] ;
 	int canWrite[DIR_SIZE]; //WILL BE 1 FOR WRITE PRIVILEDGES
 	long numOpen;
} PROCFILETABLE;
