/// *** main.cpp ***
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

#define CREATETEAM      100
#define ADDMEMBER       200
#define REMOVEMEMBER    300 
#define MATCHUP         400
#define RECRUITNATIONAL 500

#define MAX_NAME_LEN 5

extern void init();
extern void createTeam(char teamName[MAX_NAME_LEN], char memberNames[][MAX_NAME_LEN], int stats[]);
extern void addMember(char memberName[MAX_NAME_LEN], char teamName[MAX_NAME_LEN], int stat);
extern int removeMember(char memberName[MAX_NAME_LEN]);
extern int matchUp(char teamName1[MAX_NAME_LEN], char teamName2[MAX_NAME_LEN], int matchPoint);
extern int recruitNationalMember();

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

static int run()
{
	int Q;
	int cmd;
	char teamName[MAX_NAME_LEN], teamName2[MAX_NAME_LEN], memberName[MAX_NAME_LEN];
	char memberNames[3][MAX_NAME_LEN];
	int stat, matchPoint;
	int stats[3];

	init();
	int judgeMent = 1;
	int ans, ret;

	scanf("%d", &Q);
	for (int i = 1; i < Q; ++i) {
		scanf("%d", &cmd);
		switch (cmd)
		{
		case CREATETEAM:
			scanf("%s", teamName);
			scanf("%s%s%s", memberNames[0], memberNames[1], memberNames[2]);
			scanf("%d%d%d", stats, stats + 1, stats + 2);
			createTeam(teamName, memberNames, stats);
			break;
		case ADDMEMBER:
			scanf("%s%s%d", memberName, teamName, &stat);
			addMember(memberName, teamName, stat);
			break;
		case REMOVEMEMBER:
			scanf("%s", memberName);
			ret = removeMember(memberName);
			scanf("%d", &ans);
			if (ret != ans)
				judgeMent = 0;
			break;
		case MATCHUP:
			scanf("%s%s%d", teamName, teamName2, &matchPoint);
			ret = matchUp(teamName, teamName2, matchPoint);
			scanf("%d", &ans);
			if (ret != ans)
				judgeMent = 0;
			break;
		case RECRUITNATIONAL:
			ret = recruitNationalMember();
			scanf("%d", &ans);
			if (ret != ans)
				judgeMent = 0;
			break;
		}
	}

	return judgeMent;
}

int main()
{
	setbuf(stdout, NULL);

	freopen("input.txt", "r", stdin);

	int testCase, perfectScore;
	scanf("%d %d", &testCase, &perfectScore);

	for (int tc = 1; tc <= testCase; tc++) {
		int score = run() ? perfectScore : 0;
		printf("#%d %d\n", tc, score);
	}

	return 0;
}
