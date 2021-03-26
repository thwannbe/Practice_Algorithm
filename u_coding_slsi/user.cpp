/// *** user.cpp ***
#define MAX_NAME_LEN 5
#define MAX_TEAM_NR 20
#define MAX_MEMBER_NR 100000
//#define HASH_SIZE ((2 << 15) - 1)
#define HASH_SIZE 32771

int nr_teams;
int nr_members;
int hash[HASH_SIZE];

static inline int strcmp(const char str1[MAX_NAME_LEN], const char str2[MAX_NAME_LEN]);
static inline void strcpy(char dst[MAX_NAME_LEN], const char src[MAX_NAME_LEN]);
static int getTeamByName(const char teamName[MAX_NAME_LEN]);
static int getHashKey(const char memberName[MAX_NAME_LEN]);
static int getMemberByName(const char memberName[MAX_NAME_LEN]);
static int allocTeam(char teamName[MAX_NAME_LEN]);

struct priority_queue {

};

struct member {
	int id;
	char name[MAX_NAME_LEN];
	int tid;
	// for hash
	int prev_id;
	int next_id;
	int hash_key;
} members[MAX_MEMBER_NR];

struct team {
	int id;
	char name[MAX_NAME_LEN];
	priority_queue pq;

	int allocMember(char memberName[MAX_NAME_LEN], int stat) {
		int mid = nr_members;
		members[mid].id = mid;
		strcpy(members[mid].name, memberName);
		members[mid].tid = id;
		members[mid].prev_id = -1;
		members[mid].next_id = -1;
		int hash_key = members[mid].hash_key = getHashKey(memberName);
		if (hash[hash_key] < 0) hash[hash_key] = mid;
		else {
			int last_hash_mid = hash[hash_key];
			while (members[last_hash_mid].next_id >= 0)
				last_hash_mid = members[last_hash_mid].next_id;
			members[last_hash_mid].next_id = mid;
		}

	}
} teams[MAX_TEAM_NR];

void init() {
	//initTeams();
	nr_teams = 0;
	//initMembers();
	nr_members = 0;
	initHash();
}

void createTeam(char teamName[MAX_NAME_LEN], char memberNames[][MAX_NAME_LEN], int stats[]) {
	int tid = allocTeam(teamName);
	for (int i = 0; i < 3; ++i)
		teams[tid].allocMember(memberNames[i], stats[i]);
}

void addMember(char memberName[MAX_NAME_LEN], char teamName[MAX_NAME_LEN], int stat) {
	int tid = getTeamByName(teamName);
	teams[tid].allocMember(memberName, stat);
}

int removeMember(char memberName[MAX_NAME_LEN]) {
	int mid = getMemberByName(memberName);
	return members[mid].escapeTeam();
}

int matchUp(char teamName1[MAX_NAME_LEN], char teamName2[MAX_NAME_LEN], int matchPoint) {
	int tid1 = getTeamByName(teamName1);
	int tid2 = getTeamByName(teamName2);
	return teams[tid1].matchUp(tid2, matchPoint);
}

int recruitNationalMember() {
	int ret = 0;
	for (int i = 0; i < nr_teams; i++)
		ret += teams[i].recruitNationalMember();
	return ret;
}

// impl

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

static int allocTeam(char teamName[MAX_NAME_LEN]) {
	teams[nr_teams].id = nr_teams;
	strcpy(teams[nr_teams].name, teamName);
	return nr_teams++;
}
