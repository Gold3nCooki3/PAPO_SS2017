#ifndef PROGRAM_TEST_H
#define PROGRAM_TEST_H

#include <stdlib.h>
#include <stdio.h>


#include "queue.h"
#include "marketsim.h"
#include "entity.h"

/*
 *Spawns 8 entities
 */
void test_spawn(field*** market, queue_t* queue, int x, int y, int floor_count);

/*
 *print the whole queue of entities
 */
void print_queue(queue_t* queue);

/*
 * Test all functions in market.c
 */
void test_marked();

#endif
