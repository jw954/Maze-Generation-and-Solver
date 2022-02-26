#ifndef MAZEGENERATION_H
#define MAZEGENERATION_H
#include "mazegraphics.h"
#include "stack.h"
#include "set.h"
#include "grid.h"

Set<GridLocation> generateNeighbors(Grid<bool>& maze, GridLocation cur);

void generateMaze(Grid<bool>& maze);

Vector<GridLocation> vectorScramble(Vector<GridLocation> neighbors);


#endif // MAZEGENERATION_H
