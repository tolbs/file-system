#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "b_io.h"

void set_used_block(int v[], int i);
void set_free_block(int v[], int i);
int find_free_block(int v[], int i);