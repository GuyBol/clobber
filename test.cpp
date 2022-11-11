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


int main()
{
    testGridGetSet();
    testGridGetPossibleMoves();

    DBG("All test passed");
}