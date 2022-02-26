#include <iostream>
#include "console.h"
#include "testing/SimpleTest.h"
#include "maze.h"
#include "search.h"
#include "queue.h"
#include "stack.h"
#include "adtwarmup.h"
#include "mazegraphics.h"
#include "mazegeneration.h"

using namespace std;

// You are free to edit the main in any way that works
// for your testing/debugging purposes.
// We will supply our main() during grading



int main()
{
//    srand(time(NULL));

//    for (int i = 0; i < 50; i ++) {
//        int rng = rand() % 5 + 1;
//        cout << rng << "\n";
//    }

    if (runSimpleTests(SELECTED_TESTS)){
        return 0;
    }


    //Queue<int> q = {1,-3, 4, 5, -10};
    //duplicateNegatives(q);

    //Map<string, string> m = {{"Thomas", "Tom"}, {"Jan", "Jan"}, {"Margaret", "Meg"}};
    //removeMatchPairs(m);

//    Grid<bool> maze;
//    readMazeFile("res/5x7.maze", maze);
//    solveMaze(maze);

//    Grid<bool> maze1;
//    readMazeFile("res/25x33.maze", maze1);
//    solveMaze(maze1);

//    Grid<bool> maze2;
//    readMazeFile("res/33x41.maze", maze2);
//    solveMaze(maze2);


//    Map<string, Set<string>> map = readDocs("res/tiny.txt");
//    Map<string, Set<string>> map1 = buildIndex(map);



//    searchEngine("res/TEST.txt");

    searchEngine("res/website.txt");

//    Grid<bool> maze5(81, 81);
//    generateMaze(maze5);

//    Set<GridLocation> a;
//    a.add(GridLocation {0,0});
//    a.add(GridLocation {0,1});
//    a.add(GridLocation {0,2});
//    a.add(GridLocation {0,3});
//    a.add(GridLocation {0,4});
//    a.add(GridLocation {0,4});

//    a = vectorScramble(a);

//    cout << a;



//    cout << endl << "All done!" << endl;
//    cout << cleanToken("(!(x");
    return 0;
}
