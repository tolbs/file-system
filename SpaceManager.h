/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: (KAAT) Kimberly Nivon, Angela Avina Garcia, Abishek Neralla, Tolby Lam
* Student ID: 916563133(Kimberly), 917579109(Angela), 917725840(Abishek) 
*		920787020(Tolby)
* Project: File System Project
*
* File: SpaceManager.h
*
* Description: space manager set up for spaceManager.c
*
**************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "b_io.h"

void set_used_block(int v[], int i);
void set_free_block(int v[], int i);
int find_free_block(int v[], int i);
