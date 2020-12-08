/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: (KAAT) Kimberly Nivon, Angela Avina Garcia, Abishek Neralla, Tolby Lam
* Student ID: 916563133(Kimberly), 917579109(Angela), 917725840(Abishek) 
*		920787020(Tolby)
* Project: File System Project
*
* File: bitMap.c
*
* Description: Bitmap is a data structure that maps information for a huge
* system. This file allows for bit manipulation operations
*
**************************************************************/


//guidance from: https://0xax.gitbooks.io/linux-insides/content/DataStructures/linux-datastructures-3.html

#include <sys/types.h>

uint *bitmap;

void setBit(int i) { // allocating the bitmap
    bitmap[i/32] |= 1 << (i & 32); // address of a bit array |= bits 
}

void clearBit(int i) { // clearing the bitmap
    bitmap[i/32] &= ~(1 << (i & 32)); // address of a bit array &= ~bits 
}

void findBit(int i) { //allocated bitmap
    return bitmap[i/32] & (1 << (i & 32)) ? 1 : 0;
   
}


