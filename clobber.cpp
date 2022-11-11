#undef _GLIBCXX_DEBUG                // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline")
#pragma GCC target("bmi,bmi2,lzcnt,popcnt")
#pragma GCC target("movbe")
#pragma GCC target("aes,pclmul,rdrnd")
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")


#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

using namespace std;

#define DBG(stream)     cerr << stream << endl


const int MAX_NEIGHBOURS = 4;
const int MAX_GRID_CELLS = 64;


enum Player
{
    NONE,
    ME,
    ENEMY
};


struct Position
{
    int x;
    int y;

    Position(int ix, int iy): x(ix), y(iy)
    {}

    Position(): x(-1), y(-1)
    {}

    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y;
    }
};

typedef array<Position, MAX_NEIGHBOURS> bufferNeighbours_t;


class Grid
{
public:
    Grid(int size): _size(size)
    {
        for (int i = 0; i < size*size; i++)
        {
            _cells[i] = NONE;
        }
    }

    Player get(const Position& pos) const
    {
        return _cells[pos.x + pos.y * _size];
    }

    Player get(int x, int y) const
    {
        return _cells[x + y*_size];
    }

    void set(const Position& pos, Player player)
    {
        _cells[pos.x + pos.y * _size] = player;
    }

    int getPossibleMoves(const Position& pos, bufferNeighbours_t& positions) const
    {
        Player player = get(pos);
        if (player == NONE)
        {
            return 0;
        }
        else
        {
            Player other = player == ME ? ENEMY : ME;
            int count = 0;
            if (pos.x > 0 && get(pos.x-1, pos.y) == other)
            {
                positions[count++] = Position(pos.x-1, pos.y);
            }
            if (pos.x < _size-1 && get(pos.x+1, pos.y) == other)
            {
                positions[count++] = Position(pos.x+1, pos.y);
            }
            if (pos.y > 0 && get(pos.x, pos.y-1) == other)
            {
                positions[count++] = Position(pos.x, pos.y-1);
            }
            if (pos.y < _size-1 && get(pos.x, pos.y+1) == other)
            {
                positions[count++] = Position(pos.x, pos.y+1);
            }
            return count;
        }
    }

    string toString()
    {
        string str;
        for (int y = _size-1; y >= 0; y--)
        {
            for (int x = 0; x < _size; x++)
            {
                switch (get(x,y))
                {
                case NONE:
                    str += "-";
                    break;
                case ME:
                    str += "X";
                    break;
                case ENEMY:
                    str += "O";
                    break;
                }
            }
            if (y > 0)
            {
                str += "\n";
            }
        }
        return str;
    }

    int getSize() const
    {
        return _size;
    }

private:
    int _size;
    array<Player, MAX_GRID_CELLS> _cells;
};


class AI
{
public:
    AI(const Grid& grid): _grid(grid)
    {}

    // Return pair<from, to>
    pair<Position, Position> play()
    {
        return make_pair(Position(0,0), Position(1,0));
    }

    int evaluate(const Grid& grid)
    {
        int eval = 0;

        bufferNeighbours_t neighbours;

        for (int y = 0; y < grid.getSize(); y++)
        {
            for (int x = 0; x < grid.getSize(); x++)
            {
                switch (grid.get(x,y))
                {
                case NONE:
                    break;
                case ME:
                    if (grid.getPossibleMoves({x,y}, neighbours) > 0)
                    {
                        eval++;
                    }
                    break;
                case ENEMY:
                    if (grid.getPossibleMoves({x,y}, neighbours) > 0)
                    {
                        eval--;
                    }
                    break;
                }
            }
        }
        return eval;
    }

private:
    const Grid& _grid;
};


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
#ifndef LOCAL
int main()
{
    int board_size; // height and width of the board
    cin >> board_size; cin.ignore();
    string mycolor; // current color of your pieces ("w" or "b")
    cin >> mycolor; cin.ignore();

    // game loop
    while (1) {

        Grid grid{board_size};

        for (int y = board_size -1; y >= 0; y--) {
            string line; // horizontal row
            cin >> line; cin.ignore();

            int x = 0;
            for (const char& c: line)
            {
                if (c == 'w' || c == 'b')
                {
                    grid.set({x,y}, mycolor[0] == c ? Player::ME : Player::ENEMY);
                }
                ++x;
            }
        }
        string last_action; // last action made by the opponent ("null" if it's the first turn)
        cin >> last_action; cin.ignore();
        int actions_count; // number of legal actions
        cin >> actions_count; cin.ignore();

        DBG(grid.toString());

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "random" << endl; // e.g. e2e3 (move piece at e2 to e3)
    }
}
#endif