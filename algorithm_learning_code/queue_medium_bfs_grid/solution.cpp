#include <iostream>
#include <queue>
#include <vector>

struct Node {
    int row;
    int col;
    int dist;
};

static int shortest_path(const std::vector<std::vector<int>>& grid)
{
    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());

    if (grid[0][0] == 1 || grid[rows - 1][cols - 1] == 1) {
        return -1;
    }

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<Node> q;

    q.push({0, 0, 0});
    visited[0][0] = true;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        Node cur = q.front();
        q.pop();

        if (cur.row == rows - 1 && cur.col == cols - 1) {
            return cur.dist;
        }

        for (int i = 0; i < 4; ++i) {
            int nr = cur.row + dr[i];
            int nc = cur.col + dc[i];

            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                continue;
            }
            if (visited[nr][nc] || grid[nr][nc] == 1) {
                continue;
            }

            visited[nr][nc] = true;
            q.push({nr, nc, cur.dist + 1});
        }
    }

    return -1;
}

int main()
{
    std::vector<std::vector<int>> grid = {
        {0, 0, 1, 0},
        {1, 0, 1, 0},
        {0, 0, 0, 0},
        {0, 1, 1, 0}
    };

    std::cout << "shortest path=" << shortest_path(grid) << "\n";
    return 0;
}
