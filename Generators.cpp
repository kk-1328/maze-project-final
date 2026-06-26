#include "Generators.hpp"
#include <numeric>
#include <algorithm>

struct DisjointSet {
    std::vector<int> parent;
    DisjointSet(int n) {
        parent.resize(n);
        std::iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) parent[root_i] = root_j;
    }
};

static DisjointSet* dsInstance = nullptr; 

void initGenerator(std::vector<Cell>& grid, GenAlgo algo, GeneratorState& state, std::mt19937& rng) {
    for (auto& cell : grid) {
        cell.visited = false;
        cell.bfsVisited = false;
        cell.astarVisited = false;
        for (int i = 0; i < 4; ++i) cell.walls[i] = true;
    }

    state.isGenerating = true;
    state.currentIdx = 0;
    state.cellStack = std::stack<int>();
    state.edges.clear();
    state.edgeIndex = 0;

    if (algo == DFS_GEN) {
        grid[state.currentIdx].visited = true;
    } 
    else if (algo == KRUSKAL_GEN) {
        if (dsInstance) delete dsInstance;
        dsInstance = new DisjointSet(COLS * ROWS);

        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                int current = getIndex(x, y);
                int right = getIndex(x + 1, y);
                int bottom = getIndex(x, y + 1);
                if (right != -1) state.edges.push_back({current, right, 1});
                if (bottom != -1) state.edges.push_back({current, bottom, 2});
            }
        }
        std::shuffle(state.edges.begin(), state.edges.end(), rng);
    }
}

void stepGenerator(std::vector<Cell>& grid, GenAlgo algo, GeneratorState& state, std::mt19937& rng) {
    if (!state.isGenerating) return;

    if (algo == DFS_GEN) {
        std::vector<int> neighbors;
        Cell& c = grid[state.currentIdx];
        int top = getIndex(c.x, c.y - 1), right = getIndex(c.x + 1, c.y), bottom = getIndex(c.x, c.y + 1), left = getIndex(c.x - 1, c.y);
        if (top != -1 && !grid[top].visited) neighbors.push_back(top);
        if (right != -1 && !grid[right].visited) neighbors.push_back(right);
        if (bottom != -1 && !grid[bottom].visited) neighbors.push_back(bottom);
        if (left != -1 && !grid[left].visited) neighbors.push_back(left);

        if (!neighbors.empty()) {
            std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
            int nextIdx = neighbors[dist(rng)];
            state.cellStack.push(state.currentIdx);
            
            int xDiff = grid[state.currentIdx].x - grid[nextIdx].x;
            if (xDiff == 1) { 
                grid[state.currentIdx].walls[3] = false; 
                grid[nextIdx].walls[1] = false; 
            } else if (xDiff == -1) { 
                grid[state.currentIdx].walls[1] = false; 
                grid[nextIdx].walls[3] = false; 
            }
            
            int yDiff = grid[state.currentIdx].y - grid[nextIdx].y;
            if (yDiff == 1) { 
                grid[state.currentIdx].walls[0] = false; 
                grid[nextIdx].walls[2] = false; 
            } else if (yDiff == -1) { 
                grid[state.currentIdx].walls[2] = false; 
                grid[nextIdx].walls[0] = false; 
            }

            state.currentIdx = nextIdx;
            grid[state.currentIdx].visited = true;
        } else if (!state.cellStack.empty()) {
            state.currentIdx = state.cellStack.top();
            state.cellStack.pop();
        } else {
            state.isGenerating = false; 
        }
    } 
    else if (algo == KRUSKAL_GEN) {
        if (state.edgeIndex >= state.edges.size()) {
            state.isGenerating = false;
            if (dsInstance) { delete dsInstance; dsInstance = nullptr; }
            return;
        }

        const auto& edge = state.edges[state.edgeIndex++];
        if (dsInstance->find(edge.cell1) != dsInstance->find(edge.cell2)) {
            dsInstance->unite(edge.cell1, edge.cell2);
            if (edge.wallDirection == 1) {
                grid[edge.cell1].walls[1] = false; grid[edge.cell2].walls[3] = false;
            } else if (edge.wallDirection == 2) {
                grid[edge.cell1].walls[2] = false; grid[edge.cell2].walls[0] = false;
            }
        }
    }
}