#include "marketsim.h"


int main(int argc, char *argv[]){
	if (argc < 2) exit(EXIT_FAILURE);
	srand(time(NULL));

	int x, y, floor_count;
	field*** market;
	queue_t *queue = queue_new();

	market = import_market(argv[1], &x, &y, &floor_count);
	printf("x: %d\n", x);
	printf("y: %d\n", y);
	printf("floor_count: %d\n", floor_count);
	
	test_spawn(market, queue, x, y, floor_count);
	while(!queue_empty(queue)){
		work_queue(market, queue);
		print_queue(queue);
	}

	free_market(market, y, floor_count);
	queue_destroy(queue);
	return 0;
}
