#ifndef MARKEDSIM_H
#define MARKEDSIM_H

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

struct
field{
	int type;
	int content;
	int amount;
};

typedef struct field field;

field*** import_marked(char* path, int* x, int* y, int* floor_count);
void freemarked(field*** marked, int y, int floor_count);
field*** createmarked(int x, int y, int floor_count);

#endif