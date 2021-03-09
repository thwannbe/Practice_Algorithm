#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

typedef struct possible_num{
	int index[9]; // boolean exp, 1 means that index is possible
	int nr;
} PN;

int number[9][9]; // board value
int fixed[9][9]; // bool
PN pos_num[9][9];

void display_board_info() {
#if DEBUG
	int i, j;
	printf("< possible number >\n");
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++)
			printf("%d ", pos_num[i][j].nr);
		printf("\n");
	}
	printf("\n");
#endif
}

void display_board() {
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (number[i][j] > 0)
				printf("%d ", number[i][j]);
			else
				printf("* ");
		}
		printf("\n");
	}
	printf("\n");
}


/*
 * valid : check if this cell can be hold this num
 * return 0 - can't hold num
 *        1 - can hold
*/
int valid(int x, int y, int num) {
	int i, j;
	int c_x, c_y;
	
	// base exception
	if (number[y][x] == num) {
		printf("valid : this condition check is no need(x:%d,y:%d,num:%d)\n",x,y,num);
		exit(-1);
	}

	// check horizon
	for (i = 0; i < 9; i++) {
		if (number[y][i] == num) {
			return 0;
		}
	}

	// update vertical
	for (i = 0; i < 9; i++) {
		if (number[i][x] == num) {
			return 0;
		}
	}

	c_x = x - (x % 3);
	c_y = y - (y % 3);

	// update cube
	for (i = c_y; i < c_y + 3; i++) {
		for (j = c_x; j < c_x + 3; j++) {
			if (number[i][j] == num) {
				return 0;
			}
		}
	}

	return 1;
}

/*
 * set : set the number at a specific cell
 * return nothing
*/
void set(int x, int y, int num) {
	int i, j;
	int c_x, c_y;
#if DEBUG
	printf("set [%d, %d] -> %d\n", x, y, num);
#endif

	if (num <= 0 || num > 9 || x >= 9 || x < 0 || y >= 9 || y < 0) {
		printf("set : condition error(num:%d,x:%d,y:%d)\n",num,x,y);
		exit(-1);
	}

	number[y][x] = num;
	fixed[y][x] = 1;

	// update horizon
	for (i = 0; i < 9; i++) {
		if (pos_num[y][i].index[num - 1]) {
			pos_num[y][i].index[num - 1] = 0;
			pos_num[y][i].nr--;
		}
	}

	// update vertical
	for (i = 0; i < 9; i++) {
		if (pos_num[i][x].index[num - 1]) {
			pos_num[i][x].index[num - 1] = 0;
			pos_num[i][x].nr--;
		}
	}

	c_x = x - (x % 3);
	c_y = y - (y % 3);

	// update cube
	for (i = c_y; i < c_y + 3; i++) {
		for (j = c_x; j < c_x + 3; j++) {
			if (pos_num[i][j].index[num - 1]) {
				pos_num[i][j].index[num - 1] = 0;
				pos_num[i][j].nr--;
			}
		}
	}
}

/*
 * unset : free a specific cell to empty cell
 * return nothing
*/
void unset(int x, int y) {
	int i, j;
	int num;
	int c_x, c_y;
#if DEBUG
	printf("unset [%d, %d] = %d -> empty\n", x, y, number[y][x]);
#endif

	if (x >= 9 || x < 0 || y >= 9 || y < 0) {
		printf("unset : condition error(x:%d,y:%d)\n", x, y);
		exit(-1);
	}

	// update the cell
	num = number[y][x];
	fixed[y][x] = 0;
	number[y][x] = 0;

	// update horizon
	for (i = 0; i < 9; i++) {
		if (valid(i, y, num)) {
			pos_num[y][i].index[num - 1] = 1;
			pos_num[y][i].nr++;
		}
	}

	// update vertical
	for (i = 0; i < 9; i++) {
		if (valid(x, i, num)) {
			if (pos_num[i][x].index[num - 1] == 0) { // no dup
				pos_num[i][x].index[num - 1] = 1;
				pos_num[i][x].nr++;
			}
		}
	}

	c_x = x - (x % 3);
	c_y = y - (y % 3);
	
	// update cube
	for (i = c_y; i < c_y + 3; i++) {
		for (j = c_x; j < c_x + 3; j++) {
			if (valid(j, i, num)) {
				if (pos_num[i][j].index[num - 1] == 0) { // no dup
					pos_num[i][j].index[num - 1] = 1;
					pos_num[i][j].nr++;
				}
			}
		}
	}
}

void init_board() {
	FILE* in;
	int i, j;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			int k;
			fixed[i][j] = 0; // not fixed at init
			for (k = 0; k < 9; k++) {
				pos_num[i][j].index[k] = 1;
				pos_num[i][j].nr = 9;
			}
		}
	}
	
	in = fopen("input.txt", "r");
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			int tmp;
			fscanf(in, "%d", &tmp);
			if (tmp > 0) {
				set(j, i, tmp);
			}
			else {
				number[i][j] = tmp;
			}
		}
	}

	fclose(in);
}

int play() { // one play = one set
	int i, j, k;
	int ret;
	int min_pos_num = 10;
	int min_i, min_j;

	// base case : all cell is filled
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			if (fixed[i][j] == 0)
				goto DOPLAY;

	return 1; // means finding a solution

DOPLAY:
	// get a cell to set
	for (i = i; i < 9; i++) {
		for (j = j; j < 9; j++) {
			if (fixed[i][j] == 0) {
				if (pos_num[i][j].nr < min_pos_num) {
					min_i = i; min_j = j;
					min_pos_num = pos_num[i][j].nr;
				}
				if (min_pos_num == 0) {
					return 0; // stuck in a wrong placement
				}
			}
		}
	}

	if (min_pos_num == 1) {
		for (k = 0; k < 9; k++) {
			if (pos_num[min_i][min_j].index[k] == 1) {
				set(min_j, min_i, k + 1);
				ret = play();
				if (!ret)
					unset(min_j, min_i);
				break;
			}
		}
	} else { // min_pos_num >= 2
		for (k = 0; k < 9; k++) {
			if (pos_num[min_i][min_j].index[k] == 1) {
				set(min_j, min_i, k + 1);
				ret = play();
				if (ret)
					return ret;
				else
					unset(min_j, min_i);
			}
		}
	}

	return ret;
}

int main() {
	init_board();
	display_board();
	if (play()) {
		display_board();
		display_board_info();
	} else {
		printf("This has no solution.\n");
	}

	return 0;
}
