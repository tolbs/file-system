/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: KAAT
* Student ID: 916563133
* Project: Basic File System 
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


