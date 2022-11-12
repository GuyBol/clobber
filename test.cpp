#define LOCAL
#define MCTS_LOOPS_LIMIT 10000

#include "clobber.cpp"

#include <assert.h>


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
    return grid;
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

void testGridGetPossibleMoves()
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
}

void testGridCompleted()
{
    Grid grid(8);
    assert(grid.completed());
    grid.set({3,3}, ME);
    assert(grid.completed());
    grid.set({3,4}, ENEMY);
    assert(!grid.completed());
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

    // testGridGetSet();
    // testGridGetPossibleMoves();
    // testGridGetAllPossibleMoves();
    // testMcts2();

    testMcts();

    DBG("All test passed");
}
