#include <iostream>

#include "othello.h"

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

int main()
{
    othello::game game(4);

    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;
            game.flip_player();
        }
        
        print_othello_board(game, game.player());

        othello::pos p;
        while (1) {
            cout << "[" << to_symbol(game.player()) << "] play position x and y = " << endl;
            cin >> p.x >> p.y;
            if (game.can_play(p))
                break;
            print_othello_board(game, game.player());
        }

        game.place_piece(p);
    }

    print_othello_board(game);
    cout << endl << game_winner_message(game.winner()) << endl;
}
