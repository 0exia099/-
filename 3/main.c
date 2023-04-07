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
    double       g; // 해당 노드가 minheap(우선순위큐)에 들어갈때의 g(N)의 값.
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
double distance[ROWS][COLS] = {0,}; // 해당 노드의 g(N)값을 가지는 배열
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
        h.heap[i].data.g = -1;
        h.heap[i].priority = ROWS + COLS;	// min heap이므로 큰값으로 초기화
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

double g(int row, int col){	// g(N) 함수
	return distance[row][col];  // 배열에 저장된 g(N)값을 반환한다.
}
double h_star(int row, int col){	//h*(N) 함수
    // A알고리즘에서는 h*(N)<=h(N)가 성립되지 않아도 됨으로 맨하탄거리를 h*(N)으로 사용.
	return abs(EXIT_ROW - row) + abs(EXIT_COL - col);
}

void path(void)
{
	int		i, row, col, next_row, next_col;
	int 	dir = 0, found = FALSE;
	double prio = ROWS + COLS;
	double d, gN, hN;	// 우선순위큐에서 뺀노드의 g(N)과 우선순위큐에 넣을 새 노드의 g(N)과 h*(N)값을 가지는 변수
	element position;   // 우선순위큐에 넣을 노드
	element selected;   // 우선순위큐에서 뺀 노드

	mark_init();
	heap_init();	// 힙 초기화

	row = START_ROW;
	col = START_COL;
	
	mark[row][col] = 1;	// 시작좌표 방문처리
	while(dir < 8 && !found){	// 시작 좌표의 인근 좌표들 우선순위 큐에 추가.
        // g(N)의 값을 정확히 측정하기 위하여 대각선으로 1칸 이동하는 값은 루트(2)의 값을, 위아래옆은 1의 값을 준다.
        if(dir%2 == 0)
            gN = 1;
        else
            gN = ROOT2;
		next_row = row + move[dir].vert;    // 다음 노드의 좌표
		next_col = col + move[dir].horiz;   
		hN = h_star(next_row, next_col);    // 다음 노드의 h*(N)값을 구한다.
		if (next_row == EXIT_ROW && next_col == EXIT_COL)   // 답을 찾은 경우
		{
			from[next_row][next_col] = ((dir + 4) % 8); // 이전 경로로 가기위한 dir from에 저장
			found = TRUE;
		}
		else if (!maze[next_row][next_col]){    // 답을 찾지 못한 경우 중 벽이 아닌경우
			if(mark[next_row][next_col]){   // 이미 방문한 좌표의 경우
				if (g(next_row, next_col) > gN){    // 지금까지 찾은 해당 좌표와 처음의 거리보다 더 짧은 거리라면
					from[next_row][next_col] = ((dir + 4) % 8); // 더 짧은 경로로 돌아갈 수 있도록 from의 값 변경
					distance[next_row][next_col] = gN;  // g(N)의 값을 더 짧은 거리로 수정
					position.row = next_row;
					position.col = next_col;
					position.g = gN;
					prio = gN + hN; // 우선순위에 평가 함수를 사용
					heap_insert(&h, position, prio); // 우선순위 큐에 추가
				}
			}
			else{   // 처음 방문한 좌표이면
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				distance[next_row][next_col] = gN;  // g(N)값을 저장
				mark[next_row][next_col] = 1;   // 방문 표시
				position.row = next_row;
				position.col = next_col;
				position.g = gN;
				prio = gN + hN; // 우선순위에 평가 함수를 사용
				heap_insert(&h, position, prio); // 우선순위 큐에 추가
			}
		}
		++dir;
	}
	
	while (h.count != 0 && !found)	// 우선순위 큐가 비거나 답을 찾을때 까지
	{
		selected = heap_delete(&h);	// 우선순위 큐에서 가장 작은 거리를 가지는 것 꺼냄
		
		row = selected.row;
		col = selected.col;	
		d = g(row, col);    // 해당 좌표의 g(N)값을 구함
        // 만약 더 짧은 거리를 찾아 g(N)이 수정된 경우 이전에 더 큰 g(N)으로 평가 함수를 사용한 노드는 우선순위 큐에 그대로 존재
        // ->더 짧은(정확한) 거리로 계산한 노드는 이미 우선순위큐에 의해 사용됨
        // 따라서 해당 노드(우선순위큐에 넣을때 g(N)과 꺼낸고나서 g(N)이 다른 노드)는 아무런 작업을 하지 않도록한다.
        if(fabs(d - selected.g) > 1.0e-8){  // 실수의 비교는 부동소수점 형식의 한계로 오차가 발생함. 이를 반영하여 같은지 비교
            continue;   // 아무런 작업없이 continue
		}
		dir = 0;
		while (dir < 8 && !found)
		{
            if (dir % 2 == 0)
                gN = d + 1; // 큐에서 꺼낸 좌표에서 위아래좌우 이동은 거리 1증가
            else
                gN = d + ROOT2; // 대각선은 거리 루트(2) 증가
            next_row = row + move[dir].vert;    // 다음 좌표
			next_col = col + move[dir].horiz;
			hN = h_star(next_row, next_col);    // 다음 좌표의 h*(N)
			if (next_row == EXIT_ROW && next_col == EXIT_COL){  // 답을 찾은 경우
				from[next_row][next_col] = ((dir + 4) % 8);	// 이전 경로로 가기위한 dir from에 저장
				found = TRUE;
			}
			else if (!maze[next_row][next_col]) // 답을 찾지 못한 경우 중 벽이 아닌 경우
			{
				if (mark[next_row][next_col])   // 이미 방문한 좌표이면
				{
					if (g(next_row, next_col) > gN) // 기존의 g(N)보다 더 짧은 g(N)인 경우
					{
						from[next_row][next_col] = ((dir + 4) % 8); // 더 짧은 경로로 돌아갈 수 있도록 from의 값 변경
						distance[next_row][next_col] = gN;  // g(N)값 변경
						position.row = next_row;
						position.col = next_col;
						position.g = gN;
						prio = gN + hN; // 우선순위에 평가 함수를 사용
						heap_insert(&h, position, prio); // 우선순위 큐에 추가
					}
				}
				else    // 처음 방문한 좌표이면
				{
					from[next_row][next_col] = ((dir + 4) % 8); // 이전 경로로 가기위한 dir from에 저장
					distance[next_row][next_col] = gN;  // g(N) 저장
					mark[next_row][next_col] = 1;
					position.row = next_row;
					position.col = next_col;
					position.g = gN;
					prio = gN + hN; // 우선순위에 평가 함수를 사용
					heap_insert(&h, position, prio); // 우선순위 큐에 추가
				}
			}
			++dir;
		}
	}

	if (!found) {
		printf("This maze has NO PATH to Goal!!\n");
	}
	print_result_fromSt("A : ", found);
}



void main()
{
	DrawFlag = TRUE;	// if DrawFlag is TRUE, result maze is displayed.

	//print_maze();
	path();
}