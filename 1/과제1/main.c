#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define FALSE 	0
#define TRUE 	1
#define ROOT2 	1.4142

//#include "maze0.h"
//#include "maze1.h"
//#include "maze2.h"
#include "maze3.h"
// #include "maze4.h"
// #include "mazeBig.h"

#define MAX_QUEUE_SIZE COLS*ROWS	// 큐 사이즈 최대

typedef struct elm {
	short int		row;
	short int		col;
	short int		dir;
} element;

typedef struct {
	short int		vert;
	short int		horiz;
} offsets;

element queue[MAX_QUEUE_SIZE];	// 큐
int from[ROWS][COLS] = {0,};	// bfs에서 경로를 찾기위해 어디서 왔는지 나타내기위한 배열
//	이전 위치로 가기위한 dir값이 들어간다.
offsets move[8] = { {-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1} };
int	front = -1, rear = -1;
int DrawFlag = FALSE;

void print_maze()
{
	int 	i, j;
	char 	result[ROWS][COLS];

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
			result[i][j] = maze[i][j] == 1 ? '|' : ' ';

	for (i = 0; i < ROWS; i++)
	{
		for (j = 0; j < COLS; j++)
			printf("%c", result[i][j]);
		printf("\n");
	}
	printf("\n");
}

void print_result_fromSt(char title[], int found)
{
	int visit_cnt = 0;
	int goal_length = 0;
	int i, j;
	char result[ROWS][COLS];
	char buf[256];

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
		{
			result[i][j] = maze[i][j] == 1 ? '|' : ' ';
			if (mark[i][j])	// 큐에 넣는 순간 방문하는걸로 취급일 때
			{
				visit_cnt++;
				result[i][j] = '.';
			}
		}
	if (found){
		int x = EXIT_ROW;
		int y = EXIT_COL;
		int d1;
		int d2;

		result[x][y] = 'o';
		goal_length++;	// 도착지점 포함
		while ((x != START_ROW) || (y != START_COL))
		{
			// from에서 어느방향에서 왔는지 역으로 가는 dir이 있으므로 move를 사용해 이전 좌표로 이동
			d1 = move[from[x][y]].vert;
			d2 = move[from[x][y]].horiz;

			x += d1;
            y += d2;

			result[x][y] = 'o';
			goal_length++;	// 이전 좌표 경로에 포함
		}
	}
	result[START_ROW][START_COL] = 'S';
	result[EXIT_ROW][EXIT_COL] = 'G';

	if (DrawFlag)
	{
		printf("%s\n", title);
		for (i = 0; i < ROWS; i++)
		{
			for (j = 0; j < COLS; j++)
				printf("%c", result[i][j]);
			printf("\n");
		}
	}
	printf("%s\n", title);
	printf("  No. of Visited Nodes : %d\n", visit_cnt + 1);
	printf("  Length of Goal Path : %d\n\n", goal_length - 1);
}

void add(int* rear, element item)
{
	queue[++(*rear)] = item;
	return;
}

element delete(int* front, int* rear)
{
	element i = queue[++(*front)];
    return i;
}
void mark_init()
{
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			mark[i][j] = 0;
}

void path(void)		// BFS search using queue
{
	int		i, row, col, next_row, next_col;
	int 	dir, found = FALSE;
	element position;

	mark_init();

	position.row = START_ROW;
	position.col = START_COL;
	position.dir = 0; // North
	mark[position.row][position.col] = 1;
	add(&rear, position);

	while (rear != front && !found)
	{
		position = delete(&front, &rear);
		row = position.row;
		col = position.col;
		dir = 0;
		// BFS는 주변에 가능한 값 전부 큐에 넣는다.
		while (dir < 8 && !found)
		{
			next_row = row + move[dir].vert;     /* move in direction dir */
			next_col = col + move[dir].horiz;
			if (next_row == EXIT_ROW && next_col == EXIT_COL){
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				found = TRUE;
			}
			else if (!maze[next_row][next_col] && !mark[next_row][next_col])
			{
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				
				mark[next_row][next_col] = 1;
				position.row = next_row;
				position.col = next_col;
				position.dir = dir++;
				add(&rear, position);
			}
			else
				++dir;
		}
	}

	if (found) {
		position.row = EXIT_ROW;	// 출구 위치 
		position.col = EXIT_COL;
		position.dir = 0;
		add(&rear, position);
	}
	else
		printf("This maze has NO PATH to Goal!!\n");
	
	print_result_fromSt("BFS : ", found);
}


//void main()
int main()	// void면 경고뜰때가 있어서 int로 해둠.
{
	DrawFlag = TRUE;	// if DrawFlag is TRUE, result maze is displayed.

	//print_maze();
	path();	// BFS
}