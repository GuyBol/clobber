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
#include <chrono>
#include <math.h>

using namespace std;

#define DBG(stream)     cerr << stream << endl

const int MY_INFINITY = 9999999;
const int MAX_NEIGHBOURS = 4;
const int MAX_GRID_CELLS = 64;
const int MAX_POSSIBLE_MOVES = 112;
const int MAX_MINIMAX_DEPTH = 3;
const int TIMEOUT_START = 1000;
const int TIMEOUT = 150;
const double EXPLORATION = 100.;


double MyLog(unsigned int value)
{
    static array<double, 1000000> cache = {0.};
    if (cache[value] == 0.)
    {
        cache[value] = log(value);
    }
    return cache[value];
}


class Random
{
public:
    static void Init()
    {
        srand(time(nullptr));
    }

    // Return a random number in [0, max[
    static int Rand(int max)
    {
        return rand() % max;
    }
};


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


struct Move
{
    Position from;
    Position to;

    Move(): from({-1,-1}), to({-1,-1})
    {}

    Move(const Position& ifrom, const Position& ito): from(ifrom), to(ito)
    {}

    bool operator==(const Move& other) const
    {
        return from == other.from && to == other.to;
    }

    string toString() const
    {
        string str;
        str += 'a' + from.x;
        str += '1' + from.y;
        str += 'a' + to.x;
        str += '1' + to.y;
        return str;
    }
};

typedef array<Move, MAX_POSSIBLE_MOVES> bufferPossibleMoves_t;


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

    int getAllPossibleMoves(Player player, bufferPossibleMoves_t& moves) const
    {
        int count = 0;
        bufferNeighbours_t buffer;
        for (int x = 0; x < _size; x++)
        {
            for (int y = 0; y < _size; y++)
            {
                if (get(x,y) == player)
                {
                    int added = getPossibleMoves({x,y}, buffer);
                    for (int i = 0; i < added; i++)
                    {
                        moves[count + i] = Move({x,y}, buffer[i]);
                    }
                    count += added;
                }
            }
        }
        return count;
    }

    bool completed() const
    {
        bufferPossibleMoves_t buffer;
        return getAllPossibleMoves(ME, buffer) + getAllPossibleMoves(ENEMY, buffer) == 0;
    }

    Player getWinner(Player player) const
    {
        if (completed())
        {
            return player == ME ? ENEMY : ME;
        }
        else
        {
            return NONE;
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


class TreeElem
{
public:
    TreeElem(const Grid& grid, TreeElem* parent, Player player, const Move& move):
        _grid(grid),
        _parent(parent),
        _player(player),
        _move(move),
        _score(0),
        _plays(0)
    {
    }

    TreeElem* parent()
    {
        return _parent;
    }

    bool isRoot() const
    {
        return _parent == nullptr;
    }

    void setRoot()
    {
        _parent = nullptr;
    }

    vector<TreeElem*>& getChildren()
    {
        return _children;
    }
    const vector<TreeElem*>& getChildren() const
    {
        return _children;
    }

    bool isLeaf() const
    {
        return _children.empty();
    }

    const Grid& grid() const
    {
        return _grid;
    }

    const Player& player() const
    {
        return _player;
    }

    const Move& move() const
    {
        return _move;
    }

    int getAllowedMoves(bufferPossibleMoves_t& buffer) const
    {
        return _grid.getAllPossibleMoves(_player, buffer);
    }

    TreeElem* addChild(Move moveToPlay)
    {
        // TODO custom allocator for perf (and fix memory leak)
        Player nextPlayer = _player == ME ? ENEMY : ME;
        TreeElem* child = new TreeElem(_grid, this, nextPlayer, moveToPlay);
        _children.push_back(child);
        child->_grid.set(moveToPlay.from, NONE);
        child->_grid.set(moveToPlay.to, _player);
        return child;
    }

    void addScore(int score)
    {
        _score += score;
        _plays++;
    }

    int score() const
    {
        return _score;
    }

    int plays() const
    {
        return _plays;
    }

    double computeUct() const
    {
        // TODO should we consider the defeat as negative score?
        //DBG(_score << " " << _plays << " " << _parent->_plays);
        if (_plays == 0)
            return INFINITY;
        else
            return ((double)_score/(double)_plays) + EXPLORATION * sqrt(MyLog(_parent->_plays)/(double)_plays);
    }

    TreeElem* getChildWithBestUct() const
    {
        double bestUct = -INFINITY;
        TreeElem* bestChild = nullptr;
        for (TreeElem* child : _children)
        {
            double uct = child->computeUct();
            if (uct > bestUct)
            {
                bestUct = uct;
                bestChild = child;
            }
        }
        return bestChild;
    }

    TreeElem* getChildWithBestAverageScore() const
    {
        double bestScore = -INFINITY;
        TreeElem* bestChild = nullptr;
        for (TreeElem* child : _children)
        {
            if ((double)child->_score/(double)child->_plays > bestScore)
            {
                bestScore = (double)child->_score/(double)child->_plays;
                bestChild = child;
            }
        }
        return bestChild;
    }

    TreeElem* findMove(const Move& move)
    {
        for (TreeElem* child : _children)
        {
            if (child->_move == move)
            {
                return child;
            }
        }
        return nullptr;
    }

    void killBrothers()
    {
        if (!isRoot())
        {
            _parent->_children.clear();
            _parent->_children.push_back(this);
        }
    }

private:
    Grid _grid;
    TreeElem* _parent;
    vector<TreeElem*> _children;
    Player _player; // The player that will play at this stage
    Move _move; // The move that lead to this node
    int _score;
    int _plays;
};


class AI
{
public:
    AI(const Grid& grid): _grid(grid), _treeRoot(nullptr), _timeout(TIMEOUT_START)
    {}

    // Return pair<from, to>
    Move play()
    {
        // TODO remove if we want to reuse tree
        _treeRoot = nullptr;
        if (_treeRoot == nullptr)
        {
            _treeRoot = new TreeElem(_grid, nullptr, ME, Move());
        }
        Move pos = mcts(*_treeRoot);
        // After first turn, timeout is 100 ms
        _timeout = TIMEOUT;
        return pos;
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
    // Monte Carlo Tree Search
    // https://vgarciasc.github.io/mcts-viz/
    // https://www.youtube.com/watch?v=UXW2yZndl7U
    Move mcts(TreeElem& root)
    {
        DBG("mcts");
        int loops = 0;
        chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
#ifndef MCTS_LOOPS_LIMIT
        while (chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() < _timeout)
#else
        for (int i = 0; i < MCTS_LOOPS_LIMIT; i++)
#endif
        {
            TreeElem* selected = selection(root);
            TreeElem* expanded = expansion(*selected);
            int score = simulation(*expanded);
            backpropagation(*expanded, score);
            loops++;
        }
        DBG(loops << " loops");
        // Chose child with best uct
        TreeElem* bestChild = root.getChildWithBestAverageScore();
        //TreeElem* bestChild = root.getChildWithBestUct();
        DBG(bestChild->score() << "/" << bestChild->plays());
        return bestChild->move();
    }

    TreeElem* selection(TreeElem& treeElem)
    {
        if (treeElem.isLeaf())
        {
            return &treeElem;
        }
        else
        {
            return selection(*treeElem.getChildWithBestUct());
        }
    }

    TreeElem* expansion(TreeElem& treeElem)
    {
        if (treeElem.plays() > 0 || treeElem.isRoot())
        {
            // Add all possible children
            bufferPossibleMoves_t allowedMoves;
            int movesCount = treeElem.getAllowedMoves(allowedMoves);
            if (movesCount > 0)
            {
                for (int i = 0; i < movesCount; i++)
                {
                    treeElem.addChild(allowedMoves[i]);
                }
                return treeElem.getChildren()[0];
            }
            else
            {
                return &treeElem;
            }
        }
        else
        {
            return &treeElem;
        }
    }

    int simulation(TreeElem& treeElem)
    {
        Grid grid = treeElem.grid();
        Player player = treeElem.player();
        Player winner = NONE;
        bufferPossibleMoves_t allowedMoves;
        while (winner == NONE)
        {
            int allowedMovesCount = grid.getAllPossibleMoves(player, allowedMoves);
            if (allowedMovesCount == 0)
            {
                winner = player == ME ? ENEMY : ME;
            }
            else
            {
                Move picked = allowedMoves[Random::Rand(allowedMovesCount)];
                grid.set(picked.from, NONE);
                grid.set(picked.to, player);
                player = player == ME ? ENEMY : ME;
            }
        }
        switch (winner)
        {
        case ME:
            return 1;
        case ENEMY:
            return 0;
        default:
            return -MY_INFINITY;
        }
    }

    void backpropagation(TreeElem& treeElem, int score)
    {
        treeElem.addScore(score);
        TreeElem* currentElem = &treeElem;
        while (!currentElem->isRoot())
        {
            currentElem = currentElem->parent();
            currentElem->addScore(score);
        }
    }

    const Grid& _grid;
    TreeElem* _treeRoot;
    int _timeout;
};


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
#ifndef LOCAL
int main()
{
    Random::Init();

    int board_size; // height and width of the board
    cin >> board_size; cin.ignore();
    string mycolor; // current color of your pieces ("w" or "b")
    cin >> mycolor; cin.ignore();

    Grid grid{board_size};
    AI ai(grid);

    // game loop
    while (1) {

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
                else if (c == '.')
                {
                    grid.set({x,y}, NONE);
                }
                ++x;
            }
        }
        string last_action; // last action made by the opponent ("null" if it's the first turn)
        cin >> last_action; cin.ignore();
        int actions_count; // number of legal actions
        cin >> actions_count; cin.ignore();

        //DBG(grid.toString());

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << ai.play().toString() << endl; // e.g. e2e3 (move piece at e2 to e3)
    }
}
#endif