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


#define MAX_HEAP_SIZE COLS*ROWS

typedef struct elm {
	short int		row;
	short int		col;
} element;

typedef struct {
	short int		vert;
	short int		horiz;
} offsets;
// 우선순위 큐를 min 힙을 통해 사용
typedef struct heap_node	// 힙의 노드
{
    double priority;
    element data;
} node;
 
typedef struct heap_tree	// 힙
{
    int count;	// 힙 카운트
    node heap[MAX_HEAP_SIZE];	// 힙에 사용할 배열
} htree;

htree h;
// 노드들을 동적할당하여 트리를 생성하면 free를 하기 힘들어 경로를 찾기위한 이차원 배열 사용
int from[ROWS][COLS] = {0,};	// 경로를 찾기위해 어디서 왔는지 나타내기위한 배열
offsets move[8] = { {-1,0},{-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1} };

int DrawFlag = FALSE;

// 힙 초기화 함수
void heap_init(){
	int i = 0;
    h.count = 0;
    for (i = 0; i < MAX_HEAP_SIZE; i++)
    {
		h.heap[i].data.row = -1;
		h.heap[i].data.col = -1;
        h.heap[i].priority = ROWS + COLS;
    }
    return;
}

void heap_insert(htree* h, element data, double priority)	// 힙에 데이터 삽입
{
    int insert_index = h->count + 1; 
    int parent_index = 0;
    node new_node;
	new_node.priority = priority;
	new_node.data = data;
     
    while (insert_index != 1)	// 삽입 위치가 루트가 될때까지 반복
    {
        parent_index = insert_index / 2;
        if (new_node.priority >= h->heap[parent_index].priority){
			// 우선순위가 크면 break;
            break;
		}
        else{
			// 우선순위가 작으면 부모와 변경
            h->heap[insert_index] = h->heap[parent_index];
            insert_index = parent_index;
        }
    }
    h->heap[insert_index] = new_node;
    h->count++;
    return;
}
// 힙에서 꺼내는 함수
element heap_delete(htree* h)
{
    element result = h->heap[1].data;	// 루트의 데이터 꺼냄
    node last_element = h->heap[h->count]; // 루트로 올릴 마지막 데이터
    int parent_index = 1;
    int child_index = 0;
	int left_child = 0;
	int right_child = 0;
 
    while (1)
    {
		left_child = parent_index * 2;
		if (parent_index * 2 > h->count){	// 자식 노드 없음
			child_index = 0;
			break;
		}
		else if (parent_index * 2 == h->count) // 자식이 왼쪽노드 하나
			child_index = left_child;
		else	// 자식 두개
		{
			right_child = left_child + 1;
			// 오른쪽 노드의 우선순위가 더 낮으면 오른쪽이 자식(올릴 노드)
			if (h->heap[left_child].priority > h->heap[right_child].priority)
				child_index = right_child;
			else	// 아니면 왼쪽이 자식
				child_index = left_child;
		}
		if (last_element.priority < h->heap[child_index].priority)
			// 자식 노드보다 우선순위가 낮으면 끝
            break;
        else
        {
			// 아니라면 자식과 부모를 바꾸면서 밑으로
            h->heap[parent_index] = h->heap[child_index];
            parent_index = child_index;
        }
    }
    h->heap[parent_index] = last_element;
    h->count--;
    return result;
}

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
	int 	visit_cnt = 0;	// 출발과 골 포함
	int 	goal_length = 0;	// 골빼고 출발에서 골 전까지
	int 	i, j;
	char 	result[ROWS][COLS];
	char	buf[256];

	for (i = 0; i < ROWS; i++)
		for (j = 0; j < COLS; j++)
		{
			result[i][j] = maze[i][j] == 1 ? '|' : ' ';
			if (mark[i][j])
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
			goal_length++;	// 이전 좌표를 경로에 포함
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
	printf("  No. of Visited Nodes : %d\n", visit_cnt + 1);	// 도착 추가
	printf("  Length of Goal Path : %d\n\n", goal_length - 1);	//시작 빼기
}

void mark_init()
{
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLS; j++)
			mark[i][j] = 0;
}

void path(void)
{
	int		i, row, col, next_row, next_col;
	int 	dir = 0, found = FALSE;
	double prio = ROWS + COLS;
	element position;
	element selected;

	mark_init();
	heap_init();	// 힙 초기화

	row = START_ROW;
	col = START_COL;
	
	mark[row][col] = 1;	// 시작좌표 방문처리
	while(dir < 8 && !found){	// 시작 좌표의 인근 좌표들 우선순위 큐에 추가.
		next_row = row + move[dir].vert;
		next_col = col + move[dir].horiz;
		if (next_row == EXIT_ROW && next_col == EXIT_COL)
		{
			from[next_row][next_col] = ((dir + 4) % 8); // 이전 경로로 가기위한 dir from에 저장
			found = TRUE;
		}
		else if (!maze[next_row][next_col] && !mark[next_row][next_col]){
			from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
			position.row = next_row;
			position.col = next_col;
			// 우선순위를 유크리드거리로
			prio = sqrt((EXIT_ROW - next_row)*(EXIT_ROW - next_row) + (EXIT_COL - next_col)*(EXIT_COL - next_col));
			heap_insert(&h, position, prio);	// 우선순위 큐에 추가
			mark[next_row][next_col] = 1;
		}
		++dir;
	}

	while (h.count != 0 && !found)	// 우선순위 큐가 비거나 답을 찾을때 까지
	{
		selected = heap_delete(&h);	// 우선순위 큐에서 가장 작은 거리를 가지는 것 꺼냄
		row = selected.row;
		col = selected.col;
		dir = 0;
		while (dir < 8 && !found)
		{
			next_row = row + move[dir].vert;
			next_col = col + move[dir].horiz;
			if (next_row == EXIT_ROW && next_col == EXIT_COL){
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				found = TRUE;
			}
			else if (!maze[next_row][next_col] && !mark[next_row][next_col])
			{
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				position.row = next_row;
				position.col = next_col;
				// 우선순위를 유크리드거리로
				prio = sqrt((EXIT_ROW - next_row)*(EXIT_ROW - next_row) + (EXIT_COL - next_col)*(EXIT_COL - next_col));
				heap_insert(&h, position, prio);	// 우선순위 큐에 추가
				mark[next_row][next_col] = 1;
			}
			++dir;
		}
	}

	if (!found) {
		printf("This maze has NO PATH to Goal!!\n");
	}
	print_result_fromSt("Best-First(유크리드) : ", found);
}



void main()
{
	DrawFlag = TRUE;	// if DrawFlag is TRUE, result maze is displayed.

	//print_maze();
	path();
}