// https://twitter.com/roun_sa_ville/status/1344316088625016832

#include <vector>
#include <iostream>

using namespace std;

namespace othello {

enum piece_color {
    white, black, none
};

string to_string(piece_color c) {
    switch (c)
    {
    case white: return "white";
    case black: return "black";
    case none: return "none";
    default:
        return "??";
    }
}

piece_color opposite(piece_color c)
{
    switch (c)
    {
    case white:
        return black;
    case black:
        return white;
    default:
        return c;
    }
}

struct pos {
    int x, y;
};

enum direction {
    N, S, E, W
};

direction all_directions[4] = {N, S, E, W};

pos next_pos(const pos &p, direction d) {
    switch (d)
    {
    case N: return {p.x, p.y - 1};
    case S: return {p.x, p.y + 1};
    case W: return {p.x - 1, p.y};
    case E: return {p.x + 1, p.y};
    default:
        return p;        
    }
}

class game {
    int size;
    vector<piece_color> board;
    piece_color next_player;

    piece_color &get(const pos &p)
    {
        return board[p.y * size + p.x];
    }

    piece_color get(const pos &p) const
    {
        return board[p.y * size + p.x];
    }

    void flip_pieces_in_direction(piece_color player, const pos &p, direction d)
    {
        if (!can_piece_surround_in_direction(player, p, d))
            return;

        // flip pieces
        for (pos np = next_pos(p, d);
            is_position_valid(np) && get(np) == opposite(player);
            np = next_pos(np, d)) {
            get(np) = player;
        }
    }

public:
    game(int s=10)
        : size(s), board(s * s)
    {
        init();
    }

    piece_color player() const { return next_player; }

    void init()
    {
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                get({x,y}) = none;
        
        // populate middle pieces
        get({size/2-1,size/2-1}) = white;
        get({size/2,size/2}) = white;
        get({size/2,size/2-1}) = black;
        get({size/2-1,size/2}) = black;

        next_player = white;
    }

    int get_size() const { return size; }

    piece_color operator[](const pos &p) const
    {
        return board[p.y * size + p.x];
    }

    bool is_position_valid(const pos &p) const
    {
        return p.x >= 0 && p.x < size && p.y >= 0 && p.y < size;
    }

    bool can_piece_surround_in_direction(piece_color player, const pos &p, direction d) const
    {
        pos np = next_pos(p, d);
        if (!is_position_valid(np))
            return false;
        
        if (opposite(player) != get(np))
            return false;
        
        while (is_position_valid(np)) {
            if (player == get(np)) {
                return true;
            } else {
                np = next_pos(np, d);
            }
        }
        return false;
    }

    bool can_play(piece_color player, const pos &p) const
    {
        if (!is_position_valid(p))
            return false;
        
        if (get(p) != none)
            return false;
        
        for (direction d : all_directions) {
            if (can_piece_surround_in_direction(player, p, d))
                return true;
        }

        return false;
    }

    bool place_piece(const pos &p)
    {
        if (!can_play(player(), p))
            return false;
        
        for (direction d : all_directions) {
            flip_pieces_in_direction(player(), p, d);
        }

        get(p) = player();

        flip_player();

        return true;
    }

    bool is_possible_to_play(piece_color c) const
    {
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                pos p = {x, y};
                if (can_play(c, p))
                    return true;
            }
        }
        return false;
    }

    int count_pieces(piece_color c) const {
        int count = 0;
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                if (get({x,y})== c)
                    count++;
        return count;
    }

    piece_color winner() const {
        int pw = count_pieces(white);
        int pb = count_pieces(black);
        if (pb == pw)
            return none;
        return pw > pb ? white : black;
    }

    void flip_player() {
        next_player = opposite(next_player);
    }
};

}

void print_othello_board(const othello::game &board, othello::piece_color pc=othello::none)
{
    cout << "  ";
    for (int x = 0; x < board.get_size(); x++)
        cout << x;
    cout << endl;

    for (int y = 0; y < board.get_size(); y++) {
        cout << y << " ";
        for (int x = 0; x < board.get_size(); x++) {
            othello::pos p = {x,y};
            switch (board[p])
            {
            case othello::white:
                cout << "X";
                break;
            case othello::black:
                cout << "O";
                break;          
            default:
                if (pc != othello::none && board.can_play(pc, p)) {
                    cout << "!";
                } else {
                    cout << ".";
                }                
            }
        }
        cout << endl;
    }
}

int main()
{
    othello::game game(4);

    while (1) {
        if (!game.is_possible_to_play(game.player())) {
            if (!game.is_possible_to_play(opposite(game.player())))
                break;
            game.flip_player();
        }
        
        print_othello_board(game, game.player());

        othello::pos p;
        while (1) {
            cout << "[" << (game.player() == othello::white ? "X" : "O") << "] play position x and y = " << endl;    
            cin >> p.x >> p.y;
            if (game.can_play(game.player(), p))
                break;
            print_othello_board(game, game.player());
        }

        game.place_piece(p);
    }

    print_othello_board(game);
    cout << endl;
    othello::piece_color winner = game.winner();
    if (winner == othello::none)
        cout << "draw" << endl;
    else
        cout << "winner is " << othello::to_string(winner) << endl;
}
