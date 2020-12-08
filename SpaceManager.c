/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: (KAAT) Kimberly Nivon, Angela Avina Garcia, Abishek Neralla, Tolby Lam
* Student ID: 916563133(Kimberly), 917579109(Angela), 917725840(Abishek) 
*		920787020(Tolby)
* Project: File System Project
*
* File: SpaceManager.c
*
* Description: space management file , freeing our blocks
*
**************************************************************/


#include "VolumeCreator.h"
#include "SpaceManager.h"

void set_used_block(int v[], int i) {
    v[i] = 1;
}

void set_free_block(int v[], int i) {
    v[i] = 0;
}

int find_free_block(int v[], int i) {
    for(int i = 0; i < v.size() -1; i++) {
        if(v[i] == 0) {
            set_used_block(v, i);
            return i;
        }
    }
}
