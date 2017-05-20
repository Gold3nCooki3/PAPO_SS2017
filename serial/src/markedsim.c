#include "markedsim.h"

struct field***

int getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return atoi(tok);
    }
    return NULL;
}

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
	FILE *file = fopen(path, "r");
	char line[255];
	fgets(line, sizeof(line), file);
	*x = getfield(char* line, 0);
	*y = getfield(char* line, 1);
	*floor_count = getfield(char* line, 3);
	
	//check if file exists
	if (fp == NULL) exit(EXIT_FAILURE);
	
	struct field*** marked = createmarked(*x, *y, *floor_count);
	
	fclose(file);
return marked
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
if (arc < 1) exit(EXIT_FAILURE);

int x, y, floor_count;	
import_marked(argv, &x, &y, &floor_count);

freemarked(marked, y, floor_count);
}
