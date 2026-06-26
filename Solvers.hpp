#pragma once
#include "Cell.hpp"
#include <queue>
#include <map>

struct AStarNode {
    int index;
    int fScore;
    bool operator>(const AStarNode& other) const { return fScore > other.fScore; }
};

int getManhattanDistance(int idx1, int idx2);

void stepBFS(std::vector<Cell>& grid, std::queue<int>& queue, std::map<int, int>& parentMap, std::vector<int>& finalPath, bool& done, int startCell, int goalCell);

void stepAStar(std::vector<Cell>& grid, std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>>& openSet, std::map<int, int>& parentMap, std::map<int, int>& gScore, std::vector<int>& finalPath, bool& done, int startCell, int goalCell);