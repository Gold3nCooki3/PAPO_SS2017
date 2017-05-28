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
void test_spawn(meta* const mmi, queue_t* const queue,queue_t* const empty_shelfs);

/*
 *print the whole queue of entities
 */
void print_queue(queue_t* queue);

/*
 * Test all functions in market.c
 */
void test_market(char* path);

#endif
