#include "Cell.hpp"
#include "Generators.hpp"
#include "Solvers.hpp"
void drawThickLine(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end, float thickness = 3.f, sf::Color color = sf::Color::White) {
    sf::Vector2f direction = end - start;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (length == 0) return;
    
    sf::Vector2f unitDirection = direction / length;
    sf::Vector2f perpendicular(-unitDirection.y, unitDirection.x);
    
    sf::RectangleShape wall(sf::Vector2f(length, thickness));
    wall.setPosition(start - (thickness / 2.f) * perpendicular);
    
    float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159265f;
    wall.setRotation(sf::degrees(angle)); 
    wall.setFillColor(color);
    
    window.draw(wall);
}

void drawCellBackground(sf::RenderWindow& window, const Cell& cell, sf::Color color, float xOffset) {
    sf::RectangleShape rect(sf::Vector2f({(float)CELL_SIZE, (float)CELL_SIZE}));
    rect.setPosition({(float)cell.x * CELL_SIZE + xOffset, (float)cell.y * CELL_SIZE});
    rect.setFillColor(color);
    window.draw(rect);
}

void drawCellWalls(sf::RenderWindow& window, const Cell& cell, float xOffset) {
    float xpos = cell.x * CELL_SIZE + xOffset;
    float ypos = cell.y * CELL_SIZE;
    
    sf::Vector2f topLeft{xpos, ypos}, topRight{xpos + CELL_SIZE, ypos},
                 bottomRight{xpos + CELL_SIZE, ypos + CELL_SIZE}, bottomLeft{xpos, ypos + CELL_SIZE};

    sf::Color wallColor = sf::Color(255, 255, 255); 
    float thickness = 3.f;

    if (cell.walls[0]) drawThickLine(window, topLeft, topRight, thickness, wallColor);
    if (cell.walls[1]) drawThickLine(window, topRight, bottomRight, thickness, wallColor);
    if (cell.walls[2]) drawThickLine(window, bottomRight, bottomLeft, thickness, wallColor);
    if (cell.walls[3]) drawThickLine(window, bottomLeft, topLeft, thickness, wallColor);
}
int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 600}), "AI Maze Sandbox Arena - Stepping Animation");

    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<Cell> grid;
    for (int y = 0; y < ROWS; ++y)
        for (int x = 0; x < COLS; ++x)
            grid.push_back(Cell{x, y});

    GenAlgo currentGen = DFS_GEN;
    SolveAlgo currentSolve = SIDE_BY_SIDE;
    bool isPaused = false;
    int msDelay = 10; // Controls animation loop speed

    GeneratorState genState;
    initGenerator(grid, currentGen, genState, rng);

    int startCell = 0;
    int goalCell = getIndex(COLS - 1, ROWS - 1);

    std::queue<int> bfsQueue;
    std::map<int, int> bfsParent;
    std::vector<int> bfsFinalPath;
    bool bfsDone = false;

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> astarOpenSet;
    std::map<int, int> astarParent;
    std::map<int, int> gScore;
    std::vector<int> astarFinalPath;
    bool astarDone = false;

    bool solversInitialized = false;

    auto resetSolvers = [&]() {
        bfsQueue = std::queue<int>(); bfsParent.clear(); bfsFinalPath.clear(); bfsDone = false;
        astarOpenSet = std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>>();
        astarParent.clear(); astarFinalPath.clear(); gScore.clear(); astarDone = false;

        for (auto& c : grid) { c.bfsVisited = false; c.astarVisited = false; }
        bfsQueue.push(startCell); grid[startCell].bfsVisited = true;

        for (size_t i = 0; i < grid.size(); ++i) gScore[i] = 1e9;
        gScore[startCell] = 0;
        astarOpenSet.push({startCell, getManhattanDistance(startCell, goalCell)});
        grid[startCell].astarVisited = true;
        solversInitialized = true;
    };

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (event->is<sf::Event::KeyPressed>()) {
                auto keyEvent = event->getIf<sf::Event::KeyPressed>();
                if (keyEvent->code == sf::Keyboard::Key::R) {
                    initGenerator(grid, currentGen, genState, rng);
                    solversInitialized = false;
                }
                else if (keyEvent->code == sf::Keyboard::Key::G) {
                    currentGen = (currentGen == DFS_GEN) ? KRUSKAL_GEN : DFS_GEN;
                    initGenerator(grid, currentGen, genState, rng);
                    solversInitialized = false;
                }
                else if (keyEvent->code == sf::Keyboard::Key::S) {
                    if (currentSolve == SIDE_BY_SIDE) currentSolve = BFS_ONLY;
                    else if (currentSolve == BFS_ONLY) currentSolve = ASTAR_ONLY;
                    else currentSolve = SIDE_BY_SIDE;
                    if (!genState.isGenerating) resetSolvers();
                }
                else if (keyEvent->code == sf::Keyboard::Key::Space) {
                    isPaused = !isPaused;
                }
                else if (keyEvent->code == sf::Keyboard::Key::Up) {
                    msDelay = std::max(1, msDelay - 2);
                }
                else if (keyEvent->code == sf::Keyboard::Key::Down) {
                    msDelay = msDelay + 2;
                }
            }
        }

        // UNIFIED CLOCK STEP ENGINE
        if (!isPaused && clock.getElapsedTime().asMilliseconds() > msDelay) {
            clock.restart();
            
            if (genState.isGenerating) {
                stepGenerator(grid, currentGen, genState, rng);
            } else {
                if (!solversInitialized) resetSolvers();
                
                if (currentSolve == SIDE_BY_SIDE || currentSolve == BFS_ONLY)
                    stepBFS(grid, bfsQueue, bfsParent, bfsFinalPath, bfsDone, startCell, goalCell);
                if (currentSolve == SIDE_BY_SIDE || currentSolve == ASTAR_ONLY)
                    stepAStar(grid, astarOpenSet, astarParent, gScore, astarFinalPath, astarDone, startCell, goalCell);
            }
        }

        window.clear(sf::Color(15, 15, 15));
// RENDER METHOD ACCORDING TO VIEWPORT SELECTION
        if (currentSolve == SIDE_BY_SIDE || currentSolve == BFS_ONLY) {
            float offset = (currentSolve == BFS_ONLY) ? 250.f : 0.f;
            
            if (genState.isGenerating) {
                for (const auto& cell : grid) if (cell.visited) drawCellBackground(window, cell, sf::Color(30, 30, 30), offset);
                drawCellBackground(window, grid[genState.currentIdx], sf::Color(241, 196, 15), offset); 
            } else {
                for (const auto& cell : grid) if (cell.bfsVisited) drawCellBackground(window, cell, sf::Color(241, 196, 15), offset);
                for (int p : bfsFinalPath) drawCellBackground(window, grid[p], sf::Color(46, 204, 113), offset);
            }
            
            drawCellBackground(window, grid[startCell], sf::Color(52, 152, 219), offset); // Start
            drawCellBackground(window, grid[goalCell], sf::Color(231, 76, 60), offset);   // End
            for (const auto& cell : grid) drawCellWalls(window, cell, offset);
        }

        if (currentSolve == SIDE_BY_SIDE || currentSolve == ASTAR_ONLY) {
            float offset = (currentSolve == ASTAR_ONLY) ? 250.f : 500.f;
            
            if (genState.isGenerating) {
                for (const auto& cell : grid) if (cell.visited) drawCellBackground(window, cell, sf::Color(30, 30, 30), offset);
                drawCellBackground(window, grid[genState.currentIdx], sf::Color(241, 196, 15), offset); 
            } else {
                for (const auto& cell : grid) if (cell.astarVisited) drawCellBackground(window, cell, sf::Color(255, 105, 180), offset);
                for (int p : astarFinalPath) drawCellBackground(window, grid[p], sf::Color(46, 204, 113), offset);
            }
            
            drawCellBackground(window, grid[startCell], sf::Color(52, 152, 219), offset); // Start
            drawCellBackground(window, grid[goalCell], sf::Color(231, 76, 60), offset);   // End
            for (const auto& cell : grid) drawCellWalls(window, cell, offset);
        }

if (currentSolve == SIDE_BY_SIDE) drawThickLine(window, {500.f, 0.f}, {500.f, 600.f}, 5.f, sf::Color::Red);

        window.display();
    }
    return 0;
}