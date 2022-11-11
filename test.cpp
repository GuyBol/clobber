#define LOCAL

#include "clobber.cpp"

#include <assert.h>


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


int main()
{
    testGridGetSet();
    testGridGetPossibleMoves();
    testGridGetAllPossibleMoves();

    DBG("All test passed");
}
