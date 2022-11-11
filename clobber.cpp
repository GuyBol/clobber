#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;


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
};


class Grid
{
public:
    Grid(int size)
    {}

    Player get(const Position& pos) const
    {
        return NONE;
    }

    void set(const Position& pos, Player player)
    {}

    int getPossibleMoves(const Position& pos, Position* positions) const
    {}
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
    string color; // current color of your pieces ("w" or "b")
    cin >> color; cin.ignore();

    // game loop
    while (1) {
        for (int i = 0; i < board_size; i++) {
            string line; // horizontal row
            cin >> line; cin.ignore();
        }
        string last_action; // last action made by the opponent ("null" if it's the first turn)
        cin >> last_action; cin.ignore();
        int actions_count; // number of legal actions
        cin >> actions_count; cin.ignore();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "random" << endl; // e.g. e2e3 (move piece at e2 to e3)
    }
}
#endif