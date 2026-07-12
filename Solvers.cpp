#include "Solvers.hpp"
#include <cmath>

int getManhattanDistance(int idx1, int idx2) {
    return std::abs((idx1 % COLS) - (idx2 % COLS)) + std::abs((idx1 / COLS) - (idx2 / COLS));
}

void stepBFS(std::vector<Cell>& grid, std::queue<int>& queue, std::map<int, int>& parentMap, std::vector<int>& finalPath, bool& done, int startCell, int goalCell) {
    if (queue.empty() || done) return;

    int curr = queue.front(); queue.pop();
    if (curr == goalCell) {
        done = true;
        int b = goalCell;
        while (b != startCell) { finalPath.push_back(b); b = parentMap[b]; }
        finalPath.push_back(startCell);
    } else {
        int cx = grid[curr].x, cy = grid[curr].y;
        int options[4] = { getIndex(cx, cy - 1), getIndex(cx + 1, cy), getIndex(cx, cy + 1), getIndex(cx - 1, cy) };
        for (int i = 0; i < 4; ++i) {
            int n = options[i];
            if (n != -1 && !grid[curr].walls[i] && !grid[n].bfsVisited) {
                grid[n].bfsVisited = true;
                parentMap[n] = curr;
                queue.push(n);
            }
        }
    }
}

void stepAStar(std::vector<Cell>& grid, std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>>& openSet, std::map<int, int>& parentMap, std::map<int, int>& gScore, std::vector<int>& finalPath, bool& done, int startCell, int goalCell) {
    if (openSet.empty() || done) return;

    int curr = openSet.top().index; openSet.pop();
    if (curr == goalCell) {
        done = true;
        int a = goalCell;
        while (a != startCell) { 
            finalPath.push_back(a); 
            a = parentMap[a]; 
        }
        finalPath.push_back(startCell);
    } else {
        int cx = grid[curr].x, cy = grid[curr].y;
        int options[4] = { getIndex(cx, cy - 1), getIndex(cx + 1, cy), getIndex(cx, cy + 1), getIndex(cx - 1, cy) };
        for (int i = 0; i < 4; ++i) {
            int n = options[i];
            if (n != -1 && !grid[curr].walls[i]) {
                int tentative_gScore = gScore[curr] + 1;
                if (tentative_gScore < gScore[n]) {
                    parentMap[n] = curr;
                    gScore[n] = tentative_gScore;
                    int fScore = gScore[n] + getManhattanDistance(n, goalCell);
                        grid[n].astarVisited = true;
                        openSet.push({n, fScore});
                }
            }
        }
    }
}