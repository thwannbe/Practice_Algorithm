#include <iostream>
#include <vector>

using namespace std;

#define MAX_LINE_SIZE 20

int line_size;

struct line_info {
	vector<int> info;
	vector<int[MAX_LINE_SIZE]> candidate;
	bool done;
} rows[MAX_LINE_SIZE], cols[MAX_LINE_SIZE];

int answer[MAX_LINE_SIZE][MAX_LINE_SIZE];

void get_input(void) {
	cout << "size: ";
	cin >> line_size;
	for (int i = 0; i < line_size; i++) {
		cout << "[row " << i << "]" << endl;
		cout << "size: ";
		int num;
		cin >> num;
		cout << "enter numbers: ";
		for (int j = 0; j < num; j++) {
			int tmp;
			cin >> tmp;
			rows[i].info.push_back(tmp);
		}
	}
	for (int i = 0; i < line_size; i++) {
		cout << "[col " << i << "]" << endl;
		cout << "size: ";
		int num;
		cin >> num;
		cout << "enter numbers: ";
		for (int j = 0; j < num; j++) {
			int tmp;
			cin >> tmp;
			rows[i].info.push_back(tmp);
		}
	}

	// init answer
	for (int i = 0; i < line_size; i++)
		for (int j = 0; j < line_size; j++)
			answer[i][j] = -1; // init val
}

void copy_line(const int src[MAX_LINE_SIZE], int dst[MAX_LINE_SIZE]) {
	for (int i = 0; i < line_size; i++)
		dst[i] = src[i];
}

void fill_line(int line[MAX_LINE_SIZE], int start_idx, int size) {
	for (int i = 0; i < size; i++)
		line[start_idx + i] = 1;	
}

void clear_line(int line[MAX_LINE_SIZE], int start_idx, int size) {
	for (int i = 0; i < size; i++)
		line[start_idx + i] = 0;
}

void recur_gen(vector<int> info, vector<int[MAX_LINE_SIZE]> candidate, 
		int info_idx, int next_board_ptr, int[MAX_LINE_SIZE] cur_board) {
	if (info_idx == info.size()) {
		// all consumed
		int temp[MAX_LINE_SIZE];
		copy_line(cur_board, temp);
		candidate.push_back(temp);
	} else {
		for (int i = next_board_ptr; i < line_size; i++) {
			if (i + info[info_idx] <= line_size) {
				fill_line(cur_board, i, info[info_idx]);
				recur_gen(info, candidate, info_idx + 1,
						i + info[info_idx] + 1, cur_board);
				clear_line(cur_board, i, info[info_idx]);
			} else
				break;
		}
	}
}

void gen_candidate(int index, bool isRow) {
	int temp[MAX_LINE_SIZE] = {0,};
	recur_gen(isRow ? rows[index].info : cols[index].info, \
		  isRow ? rows[index].candidate : cols[index].candidate, \
		  0, 0, temp);
}

void copy_cand_to_answer(int index, const int cand[20], bool isRow) {
	if (isRow) {
		for (int i = 0; i < line_size; i++) {
			answer[index][i] = cand[i];
		}
	} else {
		for (int i = 0; i < line_size; i++) {
			answer[i][index] = cand[i];
	}
}

void fill_candidate(void) {
	// add all candidate for each rows and columns
	// if anything has only one candidate, it must be answer.
	for (int i = 0; i < line_size; i++) {
		// for row
		gen_candidate(i, true);
		if (rows[i].candidate.size() == 1)
			copy_cand_to_answer(i, (const int[20])rows[i].candidate[0], true);
		// for cols
		gen_candidate(i, false);
		if (cols[i].candidate.size() == 1)
			copy_cand_to_answer(i, (const int[20])cols[i].candidate[0], true);
	}
}

void regression(void) {
	// find all possible choices and get answer
}

void print_answer() {

}

int main()
{
	get_input();
	fill_candidate();
	regression();
	print_answer();
}
