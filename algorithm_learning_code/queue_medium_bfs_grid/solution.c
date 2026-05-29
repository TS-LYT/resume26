#include <stdbool.h>
#include <stdio.h>

#define ROWS 4
#define COLS 4
#define MAX_QUEUE (ROWS * COLS)

typedef struct {
    int row;
    int col;
    int dist;
} Node;

static int shortest_path(int grid[ROWS][COLS])
{
    if (grid[0][0] == 1 || grid[ROWS - 1][COLS - 1] == 1) {
        return -1;
    }

    bool visited[ROWS][COLS] = {{false}};
    Node queue[MAX_QUEUE];
    int front = 0;
    int rear = 0;

    queue[rear++] = (Node){0, 0, 0};
    visited[0][0] = true;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (front < rear) {
        Node cur = queue[front++];
        if (cur.row == ROWS - 1 && cur.col == COLS - 1) {
            return cur.dist;
        }

        for (int i = 0; i < 4; ++i) {
            int nr = cur.row + dr[i];
            int nc = cur.col + dc[i];

            if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) {
                continue;
            }
            if (visited[nr][nc] || grid[nr][nc] == 1) {
                continue;
            }

            visited[nr][nc] = true;
            queue[rear++] = (Node){nr, nc, cur.dist + 1};
        }
    }

    return -1;
}

int main(void)
{
    int grid[ROWS][COLS] = {
        {0, 0, 1, 0},
        {1, 0, 1, 0},
        {0, 0, 0, 0},
        {0, 1, 1, 0}
    };

    printf("shortest path=%d\n", shortest_path(grid));
    return 0;
}
