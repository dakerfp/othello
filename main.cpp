#include <iostream>
#include <memory>

#include "othello.h"
#include "ai.h"

using namespace std;

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
                if (pc != othello::none && board.can_play(p, pc)) {
                    cout << "!";
                } else {
                    cout << ".";
                }
            }
        }
        cout << endl;
    }
}

string to_symbol(othello::piece_color pc)
{
    return othello::white ? "X" : "O";
}

string game_winner_message(othello::piece_color winner)
{
    if (winner == othello::none)
        return "draw";
    else
        return "winner is " + othello::to_string(winner);
}

class human_strategy : public othello::strategy
{
public:
    human_strategy(othello::piece_color color)
        : othello::strategy(color)
    {}

    othello::pos choose_piece_position(const othello::game &game, const std::vector<othello::pos> &possible_positions) override
    {
        othello::pos p;
        while (1) {
            print_othello_board(game, game.player());
            cout << "[" << to_symbol(game.player()) << "] play position x and y = " << endl;
            cin >> p.x >> p.y;
            if (game.can_play(p))
                break;
        }
        return p;
    }
};

std::unique_ptr<othello::strategy> make_strategy_from_cin(othello::piece_color color)
{
    while (1)
    {
        unsigned int strategy_index;
        cout << "select strategy for player in " << othello::to_string(color) << endl;
        cout << "0 - human player" << endl;
        cout << "1 - random strategy" << endl;
        cin >> strategy_index;
        switch (strategy_index)
        {
        case 0:
            return move(make_unique<human_strategy>(color));
        case 1:
            return move(make_unique<othello::random_strategy>(color));
        default:
            continue; // repeat
        }
    }
}

int main()
{
    othello::game game(4);

    unique_ptr<othello::strategy> strategy_white = make_strategy_from_cin(othello::white);
    unique_ptr<othello::strategy> strategy_black = make_strategy_from_cin(othello::black);

    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;strategy strategy_black(othello::black);
            game.flip_player();
        }

        print_othello_board(game, game.player());
        switch (game.player()) {
        case othello::white:
            strategy_white->play(game);
        case othello::black:
            strategy_black->play(game);
        }
    }

    print_othello_board(game);
    cout << endl << game_winner_message(game.winner()) << endl;
}
