#include "markedsim.h"

struct field*** 
createmarked(int x, int y, int floor_count){
	struct  field *** marked = malloc(floor_count * sizeof(struct field**)); 
	for(int i = 0; i < floor_count; i++){
		struct  field** floor = malloc(y * sizeof(struct field*));
		marked[i] = floor;
		for(int j = 0; j < y; j++){
			floor[j] = malloc(x * sizeof(struct field));
		}
	}
	return marked;
}

struct field*** 
import_marked(char* path, int* x, int* y, int* floor_count){
	printf("file open: ");
	printf(path);
	printf("\n");
	FILE *file = fopen(path, "r");
	if (file == NULL) exit(EXIT_FAILURE);
	
	fscanf(file, "%d,%d,%d\n", x, y, floor_count);
	struct field*** marked = createmarked(*x, *y, *floor_count);
	for(int a = 0; a < (*floor_count); a++){
		for(int b = 0; b < (*y); b++){
			for(int c = 0; c < (*x); c++){			
				int first, second, third;
				fscanf(file, "%d,%d,%d\n", &first, &second, &third);
				marked[a][b][c].type = first;
				marked[a][b][c].content = second;
				marked[a][b][c].amount = third;
				if(first != 0 || second != 0 || third != 0){
					printf("t: %d, c: %d, a: %d\n", first,second,third);
					printf("a: %d, b: %d, c: %d\n", a,b,c);
					if(marked[a][b][c].type != first) printf("!\n");
				}
			}
		}
	}
	
	
	fclose(file);

return marked;
}

void 
freemarked(struct field*** marked, int y, int floor_count){
	for(int o = 0; o < floor_count; o++ ){
		for(int i = 0; i < y; i++){
			free(marked[o][i]);
		}
		free(marked[o]);
	}
	free(marked);
}

int
main(int argc, char *argv[]){
if (argc < 1) exit(EXIT_FAILURE);

int x, y, floor_count;	
struct field*** marked = import_marked(argv[1], &x, &y, &floor_count);
printf("x: %d\n", x);
printf("y: %d\n", y);
printf("floor_count: %d\n", floor_count);

printf("T: %d\n",marked[floor_count-1][y-1][x-1].type);
printf("C: %d\n",marked[floor_count-1][y-1][x-1].content);
printf("A: %d\n",marked[floor_count-1][y-1][x-1].amount);
printf("T: %d\n",marked[0][0][0].type);
printf("C: %d\n",marked[0][0][0].content);
printf("A: %d\n",marked[0][0][0].amount);

freemarked(marked, y, floor_count);
}
