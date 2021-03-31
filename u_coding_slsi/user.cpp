/// *** user.cpp ***
#define MAX_NAME_LEN 5
#define MAX_TEAM_NR 20
#define MAX_MEMBER_NR 100000
//#define HASH_SIZE ((2 << 15) - 1)
#define HASH_SIZE 32771

#define DEBUG_LEVEL 2

#if DEBUG_LEVEL > 0
#include <stdio.h>
#endif

// global variables
int nr_teams;
int nr_members;
int hash[HASH_SIZE];

// for string
static inline int strcmp(const char str1[MAX_NAME_LEN], const char str2[MAX_NAME_LEN]);
static inline void strcpy(char dst[MAX_NAME_LEN], const char src[MAX_NAME_LEN]);

// for name searching and hash
static int getTeamByName(const char teamName[MAX_NAME_LEN]);
static void initHash(void);
static int getHashKey(const char memberName[MAX_NAME_LEN]);
static int getMemberByName(const char memberName[MAX_NAME_LEN]);
static void addMemberToHash(int mid, int hkey);

struct team;

struct member {
	int id;
	char name[MAX_NAME_LEN];
	int tid;
	int stat;
	int pop; // TODO : possibly can be long long int
	// for hash
	int next_id;
	int hash_key;

	void init(int _id, char _name[MAX_NAME_LEN], int _tid, int _stat, int hkey) {
		id = _id;
		strcpy(name, _name);
		tid = _tid;
		stat = _stat;
		pop = 0;
		// for hash
		next_id = -1; // always last at the hash key
		hash_key = hkey;
	}

	void restart(int _tid, int _stat) {
		tid = _tid;
		stat = _stat;
		pop = 0;
	}

	int compare(member* op) { // pos : stronger, neg : weaker
		if ((stat + pop) != (op->stat + op->pop)) {
			return (stat + pop) - (op->stat + op->pop);
		}
		else {
			return stat - op->stat;
		}
	}
} members[MAX_MEMBER_NR];

struct priority_queue {
	int queue[MAX_MEMBER_NR + 1];
	int size;

	void init() { size = 0; }

#if DEBUG_LEVEL > 1
	void printQueue() {
		printf("[PRINT QUEUE]\n");
		for (int i = 1; i <= size; i *= 2) {
			for (int j = i; j < 2 * i && j <= size; j++) {
				printf("%4d(%4d) ", queue[j], members[queue[j]].stat + members[queue[j]].pop);
			}
			printf("\n");
		}
		printf("\n");
	}
#endif

	void swap(int hid1, int hid2) {
		int tmp = queue[hid2];
		queue[hid2] = queue[hid1];
		queue[hid1] = tmp;
	}

	int pop() {
		if (size <= 0) return -1;
		int ret = queue[1]; // root mid
		swap(1, size);
		size -= 1;
		int cur = 1;
		int left = 2, right = 3;
		int target = cur;
		while (left <= size) {
			if (members[queue[target]].compare(&members[queue[left]]) < 0) {
				target = left;
			}
			if (right <= size && members[queue[target]].compare(&members[queue[right]]) < 0) {
				target = right;
			}
			if (target == cur) break;
			else {
				swap(cur, target);
				cur = target;
				left = cur * 2;
				right = cur * 2 + 1;
			}
		}
#if DEBUG_LEVEL > 1
		printQueue();
#endif
		return ret;
	}

	void push(int mid) {
		int cur = ++size;
		queue[cur] = mid;
		int parent = cur / 2;
		while (parent > 0 && members[queue[cur]].compare(&members[queue[parent]]) > 0) {
			swap(cur, parent);
			cur = parent;
			parent /= 2;
		}
#if DEBUG_LEVEL > 1
		printQueue();
#endif
	}
};

struct team {
	int id;
	char name[MAX_NAME_LEN];
	int national;
	int size;
	priority_queue pq;

	void init(int tid, char tname[MAX_NAME_LEN]) {
		id = tid;
		strcpy(name, tname);
		national = -1;
		size = 0;
		pq.init();
	}

	int getSize() {
		return size;
	}

	void allocMember(char memberName[MAX_NAME_LEN], int stat) {
		int mid = getMemberByName(memberName);
		if (mid >= 0) {
			// old member
			members[mid].restart(id, stat);
			pushBackPlayer(mid);
		}
		else {
			mid = nr_members++;
			int hkey = getHashKey(memberName);
			members[mid].init(mid, memberName, id, stat, hkey);
			addMemberToHash(mid, hkey);
			pushBackPlayer(mid);
		}
		size++;
	}

	void removeMember() {
		size--;
	}

	int popTopPlayer() {
		int ret;
		do {
			ret = pq.pop();
		} while (members[ret].tid == -1);
		return ret;
	}

	void pushBackPlayer(int mid) {
		pq.push(mid);
	}

	int matchUp(team* enemy, int point) {
		if (getSize() < 3 || (getSize() == 3 && national != -1) ||
			enemy->getSize() < 3 || (enemy->getSize() == 3 && enemy->national != -1)) return -1;

		int myPlayers[3], enemyPlayers[3];
		int myPower = 0, enemyPower = 0;
		int ret;
		for (int i = 0; i < 3; i++) {
			myPlayers[i] = popTopPlayer();
			myPower += members[myPlayers[i]].stat;
			enemyPlayers[i] = enemy->popTopPlayer();
			enemyPower += members[enemyPlayers[i]].stat;
		}
		if (myPower > enemyPower) {
			for (int i = 0; i < 3; i++) {
				members[myPlayers[i]].pop += point;
				members[enemyPlayers[i]].pop -= point;
			}
			ret = myPower;
		}
		else if (myPower < enemyPower) {
			for (int i = 0; i < 3; i++) {
				members[myPlayers[i]].pop -= point;
				members[enemyPlayers[i]].pop += point;
			}
			ret = enemyPower;
		}
		else {
			ret = myPower;
		}

		for (int i = 0; i < 3; i++) {
			pushBackPlayer(myPlayers[i]);
			enemy->pushBackPlayer(enemyPlayers[i]);
		}
		return ret;
	}

	int recruitNationalMember(void) {
		if (getSize() > 0) {
			if (national > 0)
				pushBackPlayer(national);
			national = popTopPlayer();
			return members[national].stat;
		}
		return 0;
	}
} teams[MAX_TEAM_NR];

#if DEBUG_LEVEL > 0
void printStatus() {
	printf("[PRINT STATUS]\n");
	for (int i = 0; i < nr_teams; i++) {
		printf("team [%s] : \n", teams[i].name);
		for (int j = 0; j < nr_members; j++) {
			if (members[j].tid == i) {
				printf("\tmember [(%d)%s] : stat(%d) pop(%d)", 
					j, members[j].name, members[j].stat, members[j].pop);
				if (teams[i].national == j) printf("** national **");
				printf("\n");
			}
		}
	}
	printf("expelled : \n");
	for (int j = 0; j < nr_members; j++) {
		if (members[j].tid == -1) {
			printf("\tmember [(%d)%s] : stat(%d) pop(%d)\n",
				j, members[j].name, members[j].stat, members[j].pop);
		}
	}
	printf("\n");
}
#endif

void init() {
	nr_teams = 0;
	nr_members = 0;
	initHash();
}

void createTeam(char teamName[MAX_NAME_LEN], char memberNames[][MAX_NAME_LEN], int stats[]) {
#if DEBUG_LEVEL > 0
	printf("[%s]\n", __func__);
#endif
	int tid = nr_teams++;
	teams[tid].init(tid, teamName);
	for (int i = 0; i < 3; ++i)
		teams[tid].allocMember(memberNames[i], stats[i]);
#if DEBUG_LEVEL > 0
	printStatus();
#endif
}

void addMember(char memberName[MAX_NAME_LEN], char teamName[MAX_NAME_LEN], int stat) {
#if DEBUG_LEVEL > 0
	printf("[%s]\n", __func__);
#endif
	int tid = getTeamByName(teamName);
	teams[tid].allocMember(memberName, stat);
#if DEBUG_LEVEL > 0
	printStatus();
#endif
}

int removeMember(char memberName[MAX_NAME_LEN]) {
#if DEBUG_LEVEL > 0
	printf("[%s]\n", __func__);
#endif
	int mid = getMemberByName(memberName);
	if (members[mid].tid < 0) return -1;
	if (teams[members[mid].tid].national == mid) return -1;
	teams[members[mid].tid].removeMember();
	members[mid].tid = -1; // expelled
#if DEBUG_LEVEL > 0
	printStatus();
#endif
	return members[mid].pop;
}

int matchUp(char teamName1[MAX_NAME_LEN], char teamName2[MAX_NAME_LEN], int matchPoint) {
#if DEBUG_LEVEL > 0
	printf("[%s]\n", __func__);
#endif
	int tid1 = getTeamByName(teamName1);
	int tid2 = getTeamByName(teamName2);
	int ret = teams[tid1].matchUp(&teams[tid2], matchPoint);
#if DEBUG_LEVEL > 0
	printStatus();
#endif
	return ret;
}

int recruitNationalMember() {
#if DEBUG_LEVEL > 0
	printf("[%s]\n", __func__);
#endif
	int ret = 0;
	for (int i = 0; i < nr_teams; i++)
		ret += teams[i].recruitNationalMember();
#if DEBUG_LEVEL > 0
	printStatus();
#endif
	return ret;
}

// STATIC FUNCTION IMPLEMENTATION
static inline int strcmp(const char str1[MAX_NAME_LEN], const char str2[MAX_NAME_LEN]) {
	for (register int i = 0; i < 4; i++) {
		if (str1[i] == str2[i]) continue;
		else return (int)(str1[i]) - (int)(str2[i]);
	}
	return 0;
}

static inline void strcpy(char dst[MAX_NAME_LEN], const char src[MAX_NAME_LEN]) {
	for (register int i = 0; i < MAX_NAME_LEN; i++)
		dst[i] = src[i];
}

static int getTeamByName(const char teamName[MAX_NAME_LEN]) {
	for (register int i = 0; i < nr_teams; i++)
		if (strcmp(teamName, teams[i].name) == 0)
			return i;
	return -1;
}

static unsigned long long djb2(const char str[MAX_NAME_LEN]) {
	unsigned long long hash = 5381;
	for (register int i = 0; i < 4; ++i)
		hash = (hash << 5) + hash + (unsigned long long)str[i];
	return hash;
}

static unsigned long long fnv_1a(const char str[MAX_NAME_LEN]) {
	unsigned long long hash = 0xcbf29ce484222325l;
	for (register int i = 0; i < 4; ++i) {
		hash ^= (unsigned long long)str[i];
		hash *= 0x100000001b3l;
	}
	return hash;
}

static void initHash(void) {
	for (int i = 0; i < HASH_SIZE; i++)
		hash[i] = -1;
}

#if DEBUG_LEVEL > 1
static void printHash(void) {
	printf("[PRINT HASH]\n");
	for (int i = 0; i < HASH_SIZE; i++) {
		int cur = hash[i];
		if (cur >= 0) {
			printf("h[%d] ", i);
			while (cur >= 0) {
				printf("-> %d ", cur);
				cur = members[cur].next_id;
			}
			printf("\n");
		}
	}
	printf("\n");
}
#endif

static int getHashKey(const char memberName[MAX_NAME_LEN]) {
	//unsigned long long hash = djb2(memberName);
	unsigned long long hash = fnv_1a(memberName);
	return (int)(hash % HASH_SIZE);
}

static int getMemberByName(const char memberName[MAX_NAME_LEN]) {
	int hash_key = getHashKey(memberName);
	int search_mid = hash[hash_key];
	while (search_mid >= 0 && strcmp(memberName, members[search_mid].name) != 0)
		search_mid = members[search_mid].next_id;
	return search_mid;
}

static void addMemberToHash(int mid, int hkey) {
	if (hash[hkey] < 0) hash[hkey] = mid;
	else {
		int last_hash_mid = hash[hkey];
		while (members[last_hash_mid].next_id >= 0)
			last_hash_mid = members[last_hash_mid].next_id;
		members[last_hash_mid].next_id = mid;
	}
#if DEBUG_LEVEL > 1
	printHash();
#endif
}
