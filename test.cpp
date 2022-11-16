#define LOCAL
#define MCTS_LOOPS_LIMIT 10000

#include "clobber.cpp"

#include <assert.h>
#include <set>


Grid BuildGrid(const string& str)
{
    Grid grid(8);
    int i = 0;
    for (char c : str)
    {
        switch (c)
        {
        case '-':
            grid.set({i%8, 7-i/8}, NONE);
            i++;
            break;
        case 'X':
            grid.set({i%8, 7-i/8}, ME);
            i++;
            break;
        case 'O':
            grid.set({i%8, 7-i/8}, ENEMY);
            i++;
            break;
        default:
            break;
        }
    }
    grid.buildConnections();
    return grid;
}


void testRandomAccessSet()
{
    RandomAccessSet<MAX_POSSIBLE_MOVES> s;
    assert(s.size() == 0);
    assert(!s.contains(1));
    s.insert(1);
    assert(s.size() == 1);
    assert(s.contains(1));
    assert(s.at(0) == 1);
    s.insert(1);
    assert(s.size() == 1);
    assert(s.contains(1));
    assert(s.at(0) == 1);
    s.insert(100);
    assert(s.size() == 2);
    assert(s.contains(1));
    assert(s.contains(100));
    assert(s.at(0) == 1);
    assert(s.at(1) == 100);
    s.insert(101);
    assert(s.size() == 3);
    s.erase(101);
    assert(s.size() == 2);
    assert(s.contains(1));
    assert(s.contains(100));
    assert(!s.contains(101));
    assert(s.at(0) == 1);
    assert(s.at(1) == 100);
    s.erase(1);
    assert(s.size() == 1);
    assert(!s.contains(1));
    assert(s.contains(100));
    assert(s.at(0) == 100);
    s.insert(10);
    assert(s.size() == 2);
    assert(s.contains(10));
    assert(s.contains(100));
    assert(s.at(0) == 100);
    assert(s.at(1) == 10);
    s.erase(100);
    assert(s.size() == 1);
    assert(s.contains(10));
    assert(!s.contains(100));
    assert(s.at(0) == 10);
}


void testConnection()
{
    assert(Connection(0,1).hash() == 0);
    assert(Connection(1,0).hash() == 0);
    assert(Connection(0,8).hash() == 1);
    assert(Connection(1,2).hash() == 2);
    assert(Connection(4,5).hash() == 8);
    assert(Connection(62,63).hash() == 111);
    assert(Connection(0) == Connection(0,1));
    assert(Connection(1) == Connection(0,8));
    assert(Connection(2) == Connection(1,2));
    assert(Connection(8) == Connection(4,5));
    assert(Connection(111) == Connection(62,63));
    set<int> allHashes;
    for (int y = 0; y < GRID_SIZE; y++)
    {
        for (int x = 0; x < GRID_SIZE; x++)
        {
            int origin = x + y*GRID_SIZE;
            if (x < GRID_SIZE-1)
            {
                int connection = Connection(origin, x+1 + y*GRID_SIZE).hash();
                assert(allHashes.find(connection) == allHashes.end());
                allHashes.insert(connection);
            }
            if (y < GRID_SIZE-1)
            {
                int connection = Connection(origin, x + (y+1)*GRID_SIZE).hash();
                assert(allHashes.find(connection) == allHashes.end());
                allHashes.insert(connection);
            }
        }
    }
    assert(allHashes.size() == 112);
}

void testGridGetSet()
{
    Grid grid(8);
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            {
                assert(grid.get(Position(x,y)) == NONE);
            }
        }
    }
    grid.set(Position(4,4), ME);
    grid.set(Position(5,6), ENEMY);
    assert(grid.get(Position(4,4)) == ME);
    assert(grid.get(4,4) == ME);
    assert(grid.get(Position(5,6)) == ENEMY);
    grid.set(Position(4,4), NONE);
    assert(grid.get(Position(4,4)) == NONE);
}

/*void testGridGetPossibleMoves()
{
    bufferNeighbours_t buffer;
    Grid grid(8);
    assert(grid.getPossibleMoves(Position(3,3), buffer) == 0);
    grid.set(Position(3,3), ME);
    assert(grid.getPossibleMoves(Position(3,3), buffer) == 0);
    grid.set(Position(4,3), ENEMY);
    assert(grid.getPossibleMoves(Position(3,3), buffer) == 1);
    assert(buffer[0] == Position(4,3));
    grid.set(Position(3,2), ENEMY);
    grid.set(Position(4,4), ENEMY);
    assert(grid.getPossibleMoves(Position(3,3), buffer) == 2);
    for (int i = 0; i < 2; i++)
    {
        assert(buffer[0] == Position(4,3) || buffer[0] == Position(3,2));
        assert(buffer[1] == Position(4,3) || buffer[1] == Position(3,2));
    }
}

void testGridGetAllPossibleMoves()
{
    bufferPossibleMoves_t buffer;
    Grid grid(8);
    assert(grid.getAllPossibleMoves(ME, buffer) == 0);
    assert(grid.getAllPossibleMoves(ENEMY, buffer) == 0);
    grid.set(Position(3,3), ME);
    assert(grid.getAllPossibleMoves(ME, buffer) == 0);
    grid.set({4,3}, ENEMY);
    assert(grid.getAllPossibleMoves(ME, buffer) == 1);
    assert(buffer[0] == Move({3,3}, {4,3}));
    assert(grid.getAllPossibleMoves(ENEMY, buffer) == 1);
    assert(buffer[0] == Move({4,3}, {3,3}));
    grid.set({5,3}, ME);
    assert(grid.getAllPossibleMoves(ME, buffer) == 2);
    assert(buffer[0] == Move({3,3}, {4,3}));
    assert(buffer[1] == Move({5,3}, {4,3}));
    assert(grid.getAllPossibleMoves(ENEMY, buffer) == 2);
    assert(buffer[0] == Move({4,3}, {3,3}));
    assert(buffer[1] == Move({4,3}, {5,3}));
}*/

void testGridCompleted()
{
    Grid grid(8);
    grid.buildConnections();
    assert(grid.completed());
    grid.set({3,3}, ME);
    grid.buildConnections();
    assert(grid.completed());
    grid.set({3,4}, ENEMY);
    grid.buildConnections();
    assert(!grid.completed());
}

void testGridWithRandomAccessSet()
{
    // Full grid (at startup)
    Grid gridStart = BuildGrid("XOXOXOXO"
                               "OXOXOXOX"
                               "XOXOXOXO"
                               "OXOXOXOX"
                               "XOXOXOXO"
                               "OXOXOXOX"
                               "XOXOXOXO"
                               "OXOXOXOX");
    gridStart.buildConnections();
    assert(gridStart.getPossibleMovesCount() == 112);
    assert(gridStart.getPossibleMoveAt(0, ME) == Move({1,0}, {0,0}));
    assert(gridStart.getPossibleMoveAt(0, ENEMY) == Move({0,0}, {1,0}));
    assert(gridStart.getPossibleMoveAt(111, ME) == Move({6,7}, {7,7}));
    assert(gridStart.getPossibleMoveAt(111, ENEMY) == Move({7,7}, {6,7}));
    // This removes connections 0, 2, 3, and flips connections 4, 5
    gridStart.move(Move({1,0}, {2,0}));
    assert(gridStart.getPossibleMovesCount() == 107);
    // The connection 0 has been replaced by the last one
    assert(gridStart.getPossibleMoveAt(0, ME) == Move({6,7}, {7,7}));
    // The connection 1 is still there
    assert(gridStart.getPossibleMoveAt(1, ME) == Move({0,1}, {0,0}));
    // This connection must be flipped
    assert(gridStart.getPossibleMoveAt(Connection(2,3).hash(), ME) != Move({3,0},{2,0}));
    // This removes 4 connections, flips 1 to same, and flips back 1 to different
    gridStart.move(Move({3,1}, {3,0}));
    assert(gridStart.getPossibleMovesCount() == 103);
    // Check that the Connection({3,0},{2,0}) is back to the list
    bool found = false;
    for (int i = 0; i < 103; i++)
    {
        if (gridStart.getPossibleMoveAt(i, ENEMY) == Move({3,0}, {2,0}))
        {
            found = true;
            break;
        }
    }
    assert(found);

    // Grid at the end of the game
    Grid gridEnd = BuildGrid(  "--------"
                            "--------"
                            "--------"
                            "---XO---"
                            "----OX--"
                            "--------"
                            "--------"
                            "--------");
    gridEnd.buildConnections();
    assert(gridEnd.getPossibleMovesCount() == 2);
    gridEnd.move(Move({3,4},{4,4}));
    assert(gridEnd.getPossibleMovesCount() == 2);
    assert(gridEnd.getPossibleMoveAt(0, ENEMY) == Move({4,3},{5,3}));
    assert(gridEnd.getPossibleMoveAt(1, ENEMY) == Move({4,3},{4,4}));
    gridEnd.move(Move({4,3},{5,3}));
    assert(gridEnd.getPossibleMovesCount() == 0);
}

void testMcts()
{
    Grid grid = BuildGrid(  "XOXOXOXO"
                            "OXOXOXOX"
                            "XOXOXOXO"
                            "OXOXOXX-"
                            "XOXOXOXO"
                            "OXOXOXOX"
                            "XOO-XOXO"
                            "OXOXOXOX");
    DBG(grid.toString());
    AI ai(grid);
    ai.play();
}

void testMcts2()
{
    Grid grid = BuildGrid(  "-O----O-"
                            "--OOO--O"
                            "---O----"
                            "-XX--O--"
                            "-XO-----"
                            "X-O-X-O-"
                            "OOXX-OXX"
                            "--XXXXX-");
    DBG(grid.toString());
    AI ai(grid);
    assert(ai.play().toString() == "c2b2");
}


int main()
{
    Random::Init();

    // testRandomAccessSet();
    // testConnection();
    // testGridGetSet();
    // testGridCompleted();
    // // testGridGetPossibleMoves();
    // // testGridGetAllPossibleMoves();
    // testGridWithRandomAccessSet();
    // testMcts2();

    testMcts();

    DBG("All test passed");
}
