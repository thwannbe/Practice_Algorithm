#include <iostream>
#include <vector>
#include <malloc.h>

using namespace std;

#define MAX_LINE_SIZE 20
#define DEBUG 0

int line_size;

struct line_info {
	vector<int> info;
	vector<int*> candidate;
} rows[MAX_LINE_SIZE], cols[MAX_LINE_SIZE];

int answer[MAX_LINE_SIZE][MAX_LINE_SIZE];

bool answer_update = false;

bool got_answer = false;

void print_answer(void);

void get_input(void) {
	cout << "size: ";
	cin >> line_size;
	for (int i = 0; i < line_size; i++) {
		cout << "[row " << i << "] - format : [size] [numbers]+" << endl;
		int num;
		cin >> num;
		for (int j = 0; j < num; j++) {
			int tmp;
			cin >> tmp;
			rows[i].info.push_back(tmp);
		}
	}
	for (int i = 0; i < line_size; i++) {
		cout << "[column " << i << "] - format : [size] [numbers]+" << endl;
		int num;
		cin >> num;
		for (int j = 0; j < num; j++) {
			int tmp;
			cin >> tmp;
			cols[i].info.push_back(tmp);
		}
	}

	// init answer
	for (int i = 0; i < line_size; i++)
		for (int j = 0; j < line_size; j++)
			answer[i][j] = 2; // init val
}

void fill_line(int* line, int start_idx, int size) {
	for (int i = 0; i < size; i++)
		line[start_idx + i] = 1;
}

void clear_line(int* line, int start_idx, int size) {
	for (int i = 0; i < size; i++)
		line[start_idx + i] = 0;
}

int* copy_line(const int* src) {
	int* ret = (int*)malloc(sizeof(int) * line_size);
	for (int i = 0; i < line_size; i++)
		ret[i] = src[i];
	return ret;
}

void recur_gen(const vector<int> info, vector<int*> & candidate,
	int info_idx, int next_board_ptr, int* line_snap) {
	if (info_idx == info.size()) {
		// all consumed
		candidate.push_back(copy_line(line_snap));
	}
	else {
		for (int i = next_board_ptr; i < line_size; i++) {
			if (i + info[info_idx] <= line_size) {
				fill_line(line_snap, i, info[info_idx]);
				recur_gen(info, candidate, info_idx + 1,
					i + info[info_idx] + 1, line_snap);
				clear_line(line_snap, i, info[info_idx]);
			}
			else
				break;
		}
	}
}

void gen_candidate(int index, bool isRow) {
	int* temp = (int*)malloc(sizeof(int) * line_size);
	clear_line(temp, 0, line_size);
	recur_gen(isRow ? rows[index].info : cols[index].info, \
		isRow ? rows[index].candidate : cols[index].candidate, \
		0, 0, temp);
}

void copy_cand_to_answer(int index, const int* cand, bool isRow) {
	if (isRow) {
		for (int i = 0; i < line_size; i++) {
			answer[index][i] = cand[i];
		}
	}
	else {
		for (int i = 0; i < line_size; i++) {
			answer[i][index] = cand[i];
		}
	}
}

void squeeze_answer_from_cand(int index, vector<int*> v, bool isRow) {
	for (int i = 0; i < line_size; i++) {
		int prev = 2; int j;
		for (j = 0; j < v.size(); j++) {
			if (prev != 2 && v[j][i] != prev) {
				break;
			}
			prev = v[j][i];
		}
		if (j == v.size()) {
			// all same
			if (isRow) {
				answer[index][i] = prev;
			}
			else {
				answer[i][index] = prev;
			}
		}
	}
}

void fill_candidate(void) {
	// add all candidate for each rows and columns
	// if anything has only one candidate, it must be answer.
	for (int i = 0; i < line_size; i++) {
		// for rows
		gen_candidate(i, true);
		if (rows[i].candidate.size() == 1) {
			copy_cand_to_answer(i, rows[i].candidate[0], true);
			answer_update = true;
		}
		else {
			squeeze_answer_from_cand(i, rows[i].candidate, true);
		}

		// for cols
		gen_candidate(i, false);
		if (cols[i].candidate.size() == 1) {
			copy_cand_to_answer(i, cols[i].candidate[0], false);
			answer_update = true;
		}
		else {
			squeeze_answer_from_cand(i, cols[i].candidate, false);
		}
	}
}

bool is_good_to_answer(int index, bool isRow, const int* cand) {
	if (isRow) {
		for (int i = 0; i < line_size; i++)
			if (answer[index][i] != 2 && cand[i] != answer[index][i])
				return false;
	}
	else {
		for (int i = 0; i < line_size; i++)
			if (answer[i][index] != 2 && cand[i] != answer[i][index])
				return false;
	}
	return true;
}

bool update_candidate_by_answer(int index, bool isRow) {
	vector<int*> buffer;
	vector<int*>& cand = isRow ? rows[index].candidate : cols[index].candidate;
	int prev_size = cand.size();
	while (cand.size() > 0) {
		// do check till cand becomes to be empty
		if (is_good_to_answer(index, isRow, cand.back())) {
			buffer.push_back(cand.back());
		}
		cand.pop_back();
	}
	while (buffer.size() > 0) {
		// copy all valid candidate to cand again
		cand.push_back(buffer.back());
		buffer.pop_back();
	}
	return (prev_size != cand.size());
}

void first_propagate(void) {
	if (!answer_update)
		return;
	// remove any candidate violates current answer
	// if any row/col becomes to have one candidate, update answer
	// do loop till no updates

	do {
		answer_update = false;
		for (int i = 0; i < line_size; i++) {
			// for rows
			if (update_candidate_by_answer(i, true)) {
				if (rows[i].candidate.size() == 1) {
					copy_cand_to_answer(i, rows[i].candidate[0], true);
					answer_update = true;
				}
			}
			// for cols
			if (update_candidate_by_answer(i, false)) {
				if (cols[i].candidate.size() == 1) {
					copy_cand_to_answer(i, cols[i].candidate[0], false);
					answer_update = true;
				}
			}
		}
#if DEBUG
		print_answer();
#endif
	} while (answer_update);
}

void copy_answer_to_buf(int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	for (int i = 0; i < line_size; i++) {
		for (int j = 0; j < line_size; j++) {
			buf[i][j] = answer[i][j];
		}
	}
}

void copy_buf_to_answer(int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	for (int i = 0; i < line_size; i++) {
		for (int j = 0; j < line_size; j++) {
			answer[i][j] = buf[i][j];
		}
	}
}

int* get_line_from_buf(int index, bool isRow, int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	int* ret = (int*)malloc(sizeof(int) * line_size);
	for (int i = 0; i < line_size; i++) {
		if (isRow) {
			ret[i] = buf[index][i];
		}
		else {
			ret[i] = buf[i][index];
		}
	}
	return ret;
}

bool is_good_to_buf(int* line, int index, bool isRow, int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	if (isRow) {
		for (int i = 0; i < line_size; i++) {
			if (buf[index][i] != 2 && line[i] != buf[index][i]) {
				return false;
			}
		}
	}
	else {
		for (int i = 0; i < line_size; i++) {
			if (buf[i][index] != 2 && line[i] != buf[i][index]) {
				return false;
			}
		}
	}
	return true;
}

void set_line_to_buf(int* line, int index, bool isRow, int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	if (isRow) {
		for (int i = 0; i < line_size; i++) {
			buf[index][i] = line[i];
		}
	}
	else {
		for (int i = 0; i < line_size; i++) {
			buf[i][index] = line[i];
		}
	}
}

void print_buf(int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
	for (int i = 0; i < line_size; i++) {
		for (int j = 0; j < line_size; j++) {
			printf("%d ", buf[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void print_line(int* line) {
	for (int i = 0; i < line_size; i++) {
		printf("%d ", line[i]);
	}
	printf("\n");
}

void recur_interleaving(int index, bool isRow, int buf[MAX_LINE_SIZE][MAX_LINE_SIZE]) {
#if DEBUG
	printf("%s[index %d, isRow %d] ::\n", __func__, index, (int)isRow);
#endif
	if (got_answer) return;
	if (index >= line_size) {
		// got answer
		copy_buf_to_answer(buf);
		got_answer = true;
		return;
	}
#if DEBUG
	printf("buffer status[index %d, isRow %d] ::\n", index, (int)isRow);
	print_buf(buf);
#endif
	int* prev_line = get_line_from_buf(index, isRow, buf);
#if DEBUG
	printf("prev line[index %d, isRow %d] ::\n", index, (int)isRow);
	print_line(prev_line);
#endif
	for (int* entry : (isRow ? rows[index].candidate : cols[index].candidate)) {
#if DEBUG
		printf("test line[index %d, isRow %d] ::\n", index, (int)isRow);
		print_line(entry);
#endif
		if (is_good_to_buf(entry, index, isRow, buf)) {
			set_line_to_buf(entry, index, isRow, buf);
			recur_interleaving(isRow ? index : index + 1, isRow ? false : true, buf);
#if DEBUG
			printf("prev buffer status[index %d, isRow %d] ::\n", index, (int)isRow);
			print_buf(buf);
			printf("prev line[index %d, isRow %d] ::\n", index, (int)isRow);
			print_line(prev_line);
#endif
			set_line_to_buf(prev_line, index, isRow, buf);
#if DEBUG
			printf("restored buffer status[index %d, isRow %d] ::\n", index, (int)isRow);
			print_buf(buf);
#endif
		}
	}
}

void regression(void) {
	int buffer[MAX_LINE_SIZE][MAX_LINE_SIZE];
	copy_answer_to_buf(buffer);
	recur_interleaving(0, true, buffer);
}

void print_answer(void) {
	for (int i = 0; i < line_size; i++) {
		for (int j = 0; j < line_size; j++) {
			printf("%d ", answer[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main()
{
	//freopen("input.txt", "r", stdin);
	get_input();
	fill_candidate();
	printf("answer status after candidiate process ::\n");
	print_answer();
	first_propagate();
	printf("answer status after first propagate ::\n");
	print_answer();
	regression();
	printf("answer status after regression (final) ::\n");
	print_answer();
}
