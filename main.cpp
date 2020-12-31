#include <iostream>

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

int main()
{
    othello::game game(4);

    human_strategy strategy_white(othello::white);
    human_strategy strategy_black(othello::black);

    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;
            game.flip_player();
        }

        print_othello_board(game, game.player());
        switch (game.player()) {
        case othello::white:
            strategy_white.play(game);
        case othello::black:
            strategy_black.play(game);
        }
    }

    print_othello_board(game);
    cout << endl << game_winner_message(game.winner()) << endl;
}
