#pragma once
#include "Cell.hpp"
#include <random>
#include <stack>

struct Edge {
    int cell1, cell2;
    int wallDirection; 
};

struct GeneratorState {
    bool isGenerating = false;
    int currentIdx = 0;
    std::stack<int> cellStack;
    std::vector<Edge> edges;
    int edgeIndex = 0;
};

void initGenerator(std::vector<Cell>& grid, GenAlgo algo, GeneratorState& state, std::mt19937& rng);
void stepGenerator(std::vector<Cell>& grid, GenAlgo algo, GeneratorState& state, std::mt19937& rng);