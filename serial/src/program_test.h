#ifndef PROGRAM_TEST_H
#define PROGRAM_TEST_H

#include <stdlib.h>  // rand(), srand()
#include <stdio.h>
#include "markedsim.h"
#include "queue.h"

void test_spawn(field*** marked, queue_t* queue, int x, int y, int floor_count);
void print_queue(queue_t* queue);

#endif
