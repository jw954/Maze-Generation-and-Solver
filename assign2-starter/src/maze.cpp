/* (Harry) Junhao Wang
 * CS106B Programming Abstraction
 * Section Leader: Eric Bear
 * This file is the Maze portion of Assignment 1.
 * The ultimate goal is to solve a Maze using BFS and a
 * helper function. The helper function helps finding the valid
 * next move in the maze. Part of this file also includes reading
 * text files that generate mazes, and checking whether a path
 * is valid in a given maze or not.
 * The final segment of the file include provided and student tests for all functions.
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"
#include "set.h"
#include "maze.h"
#include "mazegraphics.h"
#include "testing/SimpleTest.h"
#include <cmath>
#include "mazegeneration.h"

using namespace std;


/*
 * This function finds the valid next step to take at the current location
 * in a maze. The step has to be either one step north, south, west and east
 * on the maze grid. A step is possible if the step is in the boundary of the maze
 * and if the step is not a wall.
 * @param maze: A Grid that consists of bool values, where true values represent
 *              pathways, and false values represent walls.
 * @param cur: A GridLocation that contains the current location's row and column on the maze.
 * @return: A set of possible steps to take in the form of GridLocations.
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {

    Set<GridLocation> neighbors;
    int row = cur.row;
    int col = cur.col;

    if(maze.inBounds(row + 1, col) && maze[row + 1][col]){
        GridLocation moveEast = {row + 1, col};
        neighbors.add(moveEast);
    }
    if (maze.inBounds(row - 1, col) && maze[row - 1][col]){
        GridLocation moveWest = {row - 1, col};
        neighbors.add(moveWest);
    }
    if (maze.inBounds(row, col + 1) && maze[row][col + 1]){
        GridLocation moveSouth = {row, col + 1};
        neighbors.add(moveSouth);
    }
    if (maze.inBounds(row, col - 1) && maze[row][col-1]){
        GridLocation moveNorth = {row, col - 1};
        neighbors.add(moveNorth);
    }

    return neighbors;
}


/*
 * This function checks whether a path is valid or not. If the path
 * is not valid in a given maze, then an error will be raised and
 * the description of the invalidity will be printed.
 * @param maze: A Grid that consists of bool values, where true values represent
 *              pathways, and false values represent walls.
 * @param path: A Stack of GridLocations that represent the path taken in a maze.
 */
void checkSolution(Grid<bool>& maze, Stack<GridLocation> path) {

    // Checks validaty of the exit.
    GridLocation mazeExit = {maze.numRows() - 1,  maze.numCols() - 1};
    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }


    // Checks the validity of the path between the exit and entry.
    while(path.size() > 1){
        GridLocation cur = path.pop();
        GridLocation next = path.peek();

        if (!maze.inBounds(cur.row, cur.col)){
            error("Path is not in bound");
        }
        if (!maze[cur.row][cur.col]){
            error("Part of the path is not on the pathway (Can't go through walls)");
        }
        if (abs(cur.row - next.row) > 1 || abs(cur.col - next.col) > 1){
            error("Part of the path is a not single step");
        }
        if (abs(cur.row - next.row) == 1 && abs(cur.col - next.col) == 1){
            error("Part of the path is not cardinal (Diagonal moves are not valid)");
        }

    }


    // Checks the validity of the entry.
    GridLocation mazeEntry = {0, 0};
    GridLocation pathEntry = path.pop();
    if (pathEntry != mazeEntry){    // This error will be raised if path entrance is out of bounds as well
        error("Path does not start at maze entrance");
    }
}

/*
 * This function finds the solution of the maze. The function finds the shortest
 * path between the starting point and the ending point of the maze. The top left
 * corner is the starting point and the bottom right corner is the ending point.
 * @param maze: A Grid that consists of bool values, where true values represent
 *              pathways, and false values represent walls.
 * @return: A Stack of GridLocations that represent the valid path for solving the
 *          maze.
 *          If no solutions are found, a Stack of just the entry GridLocation
 *          will be returned (This should not happen if the precondition is met).
 * Precondition: The parameter maze must be a valid maze with a valid path. This meas
 *               that there must be a solution for the maze.
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {

    MazeGraphics::drawGrid(maze);

    Stack<GridLocation> entryPath;
    GridLocation entry = {0, 0};
    GridLocation exit = {maze.numRows() - 1, maze.numCols() - 1};
    entryPath.push(entry);
    Queue<Stack<GridLocation>> queue = {entryPath};

    //Preliminary check in case the maze is 1x1
    if (entry == exit){
        return entryPath;
    }

    /* BFS Implementation */
    Set<GridLocation> visited;
    while(!queue.isEmpty()){
        Stack<GridLocation> currentPath = queue.dequeue();
        GridLocation currentLocation = currentPath.peek();

        Set<GridLocation> neighbors = generateValidMoves(maze, currentLocation);
        for (GridLocation neighbor : neighbors){
            if (!visited.contains(neighbor)){
                Stack<GridLocation> newPath = currentPath;
                newPath.push(neighbor);

                MazeGraphics::highlightPath(newPath, "blue");

                if (neighbor == exit){  // Instead of checking for currentlLocation after enqueuing and dequeuing
                    return newPath;     // Check if neighbor is the exit right away.
                }
                visited.add(neighbor);
                queue.enqueue(newPath);
            }
        }

    }

    // This function returns the path with only the
    // entry if there is no valid path in this maze.
    // This shouldn't be returned as the precondition for
    // a maze passed in must be a maze with a solution.
    // This return statement is here to avoid compiler
    // error.
    return entryPath;

}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file. A malformed maze will cause an error.
 * @param filname: A text file or maze file that consist of '-' or '@',
 *                 which makes up a "maze" that is rectagular in shape.
 * @param maze: An initialized Grid of bool.
 * Error cases added: Error for unequal row lengths.
 *                    Error for any char other than '-' or '@'
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following lines read in the data from the file into a Vector
     * of strings representing the lines of the file. We haven't talked
     * in class about what ifstreams are, so don't worry if you don't fully
     * understand what is going on here.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (string line : lines){          //compare each line's length of with the first line's length.
        if (line.length() != numCols){
            error("All maze row must have same length as all the others");
        }
    }

    for (int r = 0; r < numRows; r++) {
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("The Grid must only consists @ or -");
            }
        }
    }
}

/* This provided functions opens and read the contents of files ending
 * in a .soln extension and interprets the contents as a Stack of
 * GridLocations, populating the provided soln data structure.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    Vector<string> lines;
    readEntireFile(in, lines);

    if (lines.size() != 1){
        error("File contained too many or too few lines.");
    }

    istringstream istr(lines[0]); // Stack read does its own error-checking
    if (!(istr >> soln)) {// if not successfully read
        error("Solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateNeighbors on location in the center of 3x3 grid with no walls"){
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> neighbors = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, center));
}

PROVIDED_TEST("generateNeighbors on location on the side of 3x3 grid with no walls"){
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> neighbors = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, side));
}

PROVIDED_TEST("generateNeighbors on corner of 2x2 grid with walls"){
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> neighbors = {{1, 0}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, corner));
}

PROVIDED_TEST("checkSolution on correct path") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("checkSolution on correct path loaded from file for medium maze"){
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("checkSolution on correct path loaded from file for large maze"){
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}


PROVIDED_TEST("checkSolution on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };

    EXPECT_ERROR(checkSolution(maze, not_end_at_exit));
    EXPECT_ERROR(checkSolution(maze, not_begin_at_entry));
    EXPECT_ERROR(checkSolution(maze, go_through_wall));
    EXPECT_ERROR(checkSolution(maze, teleport));
}


PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x35") {
    Grid<bool> maze;
    readMazeFile("res/21x35.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(checkSolution(maze, soln));
}

PROVIDED_TEST("Test readMazeFile on valid file 2x2.maze") {
    Grid<bool> maze;

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(readMazeFile("res/2x2.maze", maze));
}

PROVIDED_TEST("Test readMazeFile on valid file 5x7.maze") {
    Grid<bool> maze;

    // We expect that this line of code will execute without raising
    // an exception
    EXPECT_NO_ERROR(readMazeFile("res/5x7.maze", maze));
}

PROVIDED_TEST("readMazeFile on nonexistent file should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/nonexistent_file", g));
}

PROVIDED_TEST("readMazeFile on malformed file should raise an error") {
    Grid<bool> g;

    EXPECT_ERROR(readMazeFile("res/malformed.maze", g));
}


/* * * * * STUDENT TEST CASES * * * * */

STUDENT_TEST("generateNeighbors on location on the middle of 4x4 grid with walls on north and south"){
    Grid<bool> maze = {{true, true, false, true},
                       {true, true, true, true},
                       {true, true, false, true},
                       {true, true, true, true}};
    GridLocation center = {1, 2};
    Set<GridLocation> neighbors = {{1, 1}, {1, 3}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, center));
}

STUDENT_TEST("generateNeighbors on location on the middle of 4x4 grid with walls on west and east"){
    Grid<bool> maze = {{true, true, true, true},
                       {true, false, true, false},
                       {true, true, true, true},
                       {true, true, true, true}};
    GridLocation middle = {1, 2};
    Set<GridLocation> neighbors = {{0, 2}, {2, 2}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, middle));
}

STUDENT_TEST("generateNeighbors on location on the upper left corner of 4x4 grid with walls east"){
    Grid<bool> maze = {{true, false, true, true},
                       {true, false, true, false},
                       {true, true, true, true},
                       {true, true, true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> neighbors = {{1, 0}};

    EXPECT_EQUAL(neighbors, generateValidMoves(maze, corner));
}

STUDENT_TEST("checkSolution on invalid path should raise error with larger maze and more cases") {
    Grid<bool> maze = {{true, false, true, true},
                       {true, true, true, false},
                       {true, false, true, false},
                       {false, true, true, true}};
    Stack<GridLocation> not_end_at_exit = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {0, 3} };
    Stack<GridLocation> not_begin_at_entry = {{1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {3, 3} };
    Stack<GridLocation> not_begin_at_entry_and_not_in_bound = {{1, -1},{1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {3, 3} };
    Stack<GridLocation> go_through_wall = { {0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {3, 3} };
    Stack<GridLocation> teleport = { {0 ,0}, {3, 3} };
    Stack<GridLocation> not_in_bound = {{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {4, 2}, {3, 2}, {3, 3}};

    EXPECT_ERROR(checkSolution(maze, not_end_at_exit));
    EXPECT_ERROR(checkSolution(maze, not_begin_at_entry));
    EXPECT_ERROR(checkSolution(maze, not_begin_at_entry_and_not_in_bound));
    EXPECT_ERROR(checkSolution(maze, go_through_wall));
    EXPECT_ERROR(checkSolution(maze, teleport));
    EXPECT_ERROR(checkSolution(maze, not_in_bound));
}

STUDENT_TEST("checkSolution and solveMaze"){
    Grid<bool> maze2;
    readMazeFile("res/33x41.maze", maze2);
    Stack<GridLocation> path = solveMaze(maze2);

    EXPECT_NO_ERROR(checkSolution(maze2, path));

    Grid<bool> maze3;
    readMazeFile("res/33x41.maze", maze3);
    Stack<GridLocation> path1 = solveMaze(maze3);

    EXPECT_NO_ERROR(checkSolution(maze3, path1));
}

STUDENT_TEST("readMazeFile on malformed file should raise an error"){
    Grid<bool> maze;
    //EXPECT_ERROR(readMazeFile("res/malformedstudent.txt", maze));

    Grid<bool> maze1;
    //EXPECT_ERROR(readMazeFile("res/malformedstudent2.txt", maze1));

    Grid<bool> maze2;
    //EXPECT_ERROR(readMazeFile("res/malformedstudent3.txt", maze2));

    Grid<bool> maze3;
    //EXPECT_ERROR(readMazeFile("res/malformedstudent5.txt", maze3));
}



/* This test case requires the mazegeneration file to work
 * The STUDENT_TEST in the mazegeneration file has a similar
 * test case to this one.
 */
//STUDENT_TEST("generateMaze + solveMaze + checkSolution(THE DREAM!!!!! WONBO COMBO)"){
//    Grid<bool> maze(75, 75); //both column and row have to be odd.
//    generateMaze(maze);
//    Stack<GridLocation> path = solveMaze(maze);
//    EXPECT_NO_ERROR(checkSolution(maze, path));

//    Grid<bool> maze2(51, 67); //both column and row have to be odd.
//    generateMaze(maze2);
//    Stack<GridLocation> path2 = solveMaze(maze2);
//    EXPECT_NO_ERROR(checkSolution(maze2, path2));

//}
