#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

const int CELL_SIZE = 20; 
const int COLS = 500 / CELL_SIZE; 
const int ROWS = 600 / CELL_SIZE; 

enum GenAlgo { DFS_GEN, KRUSKAL_GEN };
enum SolveAlgo { SIDE_BY_SIDE, BFS_ONLY, ASTAR_ONLY };

struct Cell {
    int x, y;
    bool walls[4] = {true, true, true, true}; 
    bool visited = false;
    bool bfsVisited = false;
    bool astarVisited = false;
};

inline int getIndex(int x, int y) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return -1;
    return x + y * COLS;
}

// Global drawing helpers
void drawThickLine(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, float thickness, sf::Color color);
void drawCellBackground(sf::RenderWindow& window, const Cell& cell, sf::Color color, float xOffset);
void drawCellWalls(sf::RenderWindow& window, const Cell& cell, float xOffset);