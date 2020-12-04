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