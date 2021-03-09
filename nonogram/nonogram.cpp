#include <iostream>
#include <vector>

using namespace std;

#define MAX_LINE_SIZE 20

int line_size;

struct line_info {
	vector<int> info;
	vector<int[20]> candidate;
	bool done;
} rows[MAX_LINE_SIZE], cols[MAX_LINE_SIZE];

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
}

void init_search(void) {
	// find possible line to be done with initial information
	// doing until no more update
}

void regression(void) {
	// find all possible choices and get answer
}

void print_answer() {

}

int main()
{
	get_input();
	init_search();
	regression();
	print_answer();
}
