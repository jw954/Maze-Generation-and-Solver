/* (Harry) Junhao Wang
 * CS106B Programming Abstraction
 * Section Leader: Eric Bear
 * This file is the extension of the maze portion.
 * This file attempts to create a maze generator
 * using backtracking method according to wikipedia.
 *
 * Unfortunately, this implemtation has some limitations
 * so the mazes created may have
 * a very easy solution (Dependent on RNG). Also, the maze
 * grid must have odd columns and rows.
 * Moreover, larger mazes tend to work better than smaller
 * mazes in terms of complexity.
 * I would love to have some feedbacks to improve this
 * maze removing algorithm.
 * The generated maze also does not look as sophisticated
 * as the ones from the res folder.
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "stack.h"
#include "set.h"
#include "maze.h"
#include "mazegraphics.h"
#include "testing/SimpleTest.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

/* This function attempts to generate the neighbors of a cell
 * This function also tries to create a vector that is randomized
 * in terms of order of neighbors inside. The parameters are, a maze,
 * and the current location in the maze.
 * A vector of all the neighbors will be returned. Note that every cell
 * is seperated by a wall.
 * (I tried to use a switch statement, but I never used it before.
 *  It would be great if there is another better way to shuffle a vector.
 *  I tried to use some other shuffle functions I found from cpp reference
 *  but it didn't work/I wasn't sure how to use it.)
 */
Vector<GridLocation> generateNeighbors(Grid<bool>& maze, GridLocation cur) {
    Vector<GridLocation> neighbors;
    int row = cur.row;
    int col = cur.col;

    srand(time(NULL));               // This line is borrowed from stackoverflow to allow rand() to work properly
    int rng = rand() % 5 + 1;        // The person who wrote the post is Robert Mason and GoBusto.

    // Randomizing the order of GridLocation in the Vector. (Shuffling)
    switch (rng){
    case 1 : {
        if(maze.inBounds(row + 2, col)){
            GridLocation east = {row + 2, col};
            neighbors.add(east);
        }
        if (maze.inBounds(row - 2, col)){
            GridLocation west = {row - 2, col};
            neighbors.add(west);
        }
        if (maze.inBounds(row, col - 2)){
            GridLocation south = {row, col - 2};
            neighbors.add(south);
        }
        if (maze.inBounds(row, col + 2)){
            GridLocation north = {row, col + 2};
            neighbors.add(north);
        }
        break;
    }
    case 2: {
        if (maze.inBounds(row, col - 2)){
            GridLocation south = {row, col - 2};
            neighbors.add(south);
        }
        if (maze.inBounds(row, col + 2)){
            GridLocation north = {row, col + 2};
            neighbors.add(north);
        }
        if (maze.inBounds(row - 2, col)){
            GridLocation west = {row - 2, col};
            neighbors.add(west);
        }
        if(maze.inBounds(row + 2, col)){
            GridLocation east = {row + 2, col};
            neighbors.add(east);
        }
        break;
    }
    case 3: {
        if (maze.inBounds(row, col - 2)){
            GridLocation south = {row, col - 2};
            neighbors.add(south);
        }
        if (maze.inBounds(row, col + 2)){
            GridLocation north = {row, col + 2};
            neighbors.add(north);
        }
        if (maze.inBounds(row - 2, col)){
            GridLocation west = {row - 2, col};
            neighbors.add(west);
        }
        if(maze.inBounds(row + 2, col)){
            GridLocation east = {row + 2, col};
            neighbors.add(east);
        }
        break;
    }
    case 4:{
        if (maze.inBounds(row - 2, col)){
            GridLocation west = {row - 2, col};
            neighbors.add(west);
        }
        if(maze.inBounds(row + 2, col)){
            GridLocation east = {row + 2, col};
            neighbors.add(east);
        }
        if (maze.inBounds(row, col + 2)){
            GridLocation north = {row, col + 2};
            neighbors.add(north);
        }
        if (maze.inBounds(row, col - 2)){
            GridLocation south = {row, col - 2};
            neighbors.add(south);
        }
        break;
    }
    case 5: {
        if (maze.inBounds(row - 2, col)){
            GridLocation west = {row - 2, col};
            neighbors.add(west);
        }
        if (maze.inBounds(row, col + 2)){
            GridLocation north = {row, col + 2};
            neighbors.add(north);
        }
        if (maze.inBounds(row, col - 2)){
            GridLocation south = {row, col - 2};
            neighbors.add(south);
        }
        if(maze.inBounds(row + 2, col)){
            GridLocation east = {row + 2, col};
            neighbors.add(east);
        }
        break;
    }
    }
    return neighbors;
}

/* This function attempts to take a bool Grid/maze and create
 * a maze from it.
 * Sometimes, a very easy solution gets created.
 * This function uses the backtracking way with an explicit stack described on Wikipedia
 * to try to generate a maze. (Apparently the stack way is better than the recursive way)
 * Essentailly, this function first makes a huge tic tac toe like looking grid, and removes
 * walls between some empty cell to create pathways.
 * This function is more likely to produce a more complex/difficult maze
 * when the size of the maze is larger.
 * The drawGrid function shows the maze drawn in real time.
 * The parameter is a maze made of Grid of bools.
 * Precondition: Maze must have odd number of columns and odd number of rows.
 */
void generateMaze(Grid<bool>& maze){

    // Make maze so that every cell/pathway is separated by a wall.
    // The initialized maze looks like a tic tac toe board
    // with way more cells.
    for (int i = 0; i < maze.numRows(); i++){
        for(int j = 0; j < maze.numCols(); j++){
            if (i % 2 == 0 && j % 2 == 0) {
                maze[i][j] = true;
            } else {
                maze[i][j] = false;
            }
        }
    }

    /* BackTracking Implementation DFS Using a Stack */
    Set<GridLocation> visited;
    Stack<GridLocation> cells; // Cells to check during backtracking

    GridLocation entry = {0, 0};
    cells.push(entry);
    visited.add(entry);

    while (!cells.isEmpty()){
        GridLocation currentCell = cells.pop();
        Vector<GridLocation> neighbors = generateNeighbors(maze, currentCell);
        for (GridLocation neighbor : neighbors){
            if (!visited.contains(neighbor)){
                cells.push(currentCell);
                maze[(neighbor.row + currentCell.row)/2][(neighbor.col + currentCell.col)/2] = true; // Clear the wall between the cells.
                visited.add(neighbor);
                cells.push(neighbor);
                MazeGraphics::drawGrid(maze);
                break;
            }
        }
    }
    MazeGraphics::drawGrid(maze);


}

/* * * * * TEST CASES * * * * */

STUDENT_TEST("generateMaze + solveMaze + checkSolution(THE DREAM!!!!! WONBO COMBO)"){
    Grid<bool> maze(85, 85); //both column and row have to be odd.
    generateMaze(maze);
    Stack<GridLocation> path = solveMaze(maze);
    EXPECT_NO_ERROR(checkSolution(maze, path));

    Grid<bool> maze2(51, 51); //both column and row have to be odd.
    generateMaze(maze2);
    Stack<GridLocation> path2 = solveMaze(maze2);
    EXPECT_NO_ERROR(checkSolution(maze2, path2));

    Grid<bool> maze3(1, 1); //both column and row have to be odd.
    generateMaze(maze3);
    Stack<GridLocation> path3 = solveMaze(maze3);
    EXPECT_NO_ERROR(checkSolution(maze3, path3));
}
