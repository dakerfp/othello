#include <iostream>
#include <memory>

#include "othello.h"
#include "ai.h"

using namespace std;

string to_symbol(othello::piece_color pc)
{
    return pc == othello::white ? "X" : "O";
}

string game_winner_message(othello::piece_color winner)
{
    if (winner == othello::none)
        return "draw";
    else
        return "winner is " + othello::to_string(winner);
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
            case othello::black:
                cout << to_symbol(board[p]);
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

std::unique_ptr<othello::strategy> make_strategy_from_index(othello::piece_color color, unsigned int index)
{
    switch (index)
    {
    case 0:
        return move(make_unique<human_strategy>(color));
    case 1:
        return move(make_unique<othello::random_strategy>(color));
    default:
        return nullptr;
    }
}

void print_strategy_indexes()
{
    cout << "select strategy for player:" << endl;
    cout << "0 - human player" << endl;
    cout << "1 - random strategy" << endl;
}

vector<string> argv_to_args(int argc, char* argv[])
{
    vector<string> args(argc - 1);
    for (int i = 1; i < argc; i++) {
        args[i-1] = argv[i];
    }
    return args;
}

unsigned int parse_strategy_index_arg(const string &arg)
{
    unsigned int strategy = atoi(arg.c_str());
    if (strategy > 2)
        strategy = 0;
    return strategy;
}

unsigned int parse_arg_othello_board_size(const string &arg)
{
    unsigned int size = atoi(arg.c_str());    
    if (size < 4)
        size = size;
    return size;
}

unsigned int arg_white_strategy = 0;
unsigned int arg_black_strategy = 0;
unsigned int arg_othello_board_size = 10;

bool parse_args(vector<string> args)
{
    for (int i = 0; i < args.size(); i++) {
        if (string(args[i]) == "help") {
            return true;
        }
        if (args[i] == "--white" || args[i] == "-w") {
            if (i + 1 == args.size()) {
                cerr << "white argument requires a value" << endl;
                print_strategy_indexes();
                return false;
            }
            arg_white_strategy = parse_strategy_index_arg(args[++i]);
        } else if (args[i] == "--black" || args[i] == "-b") {
            if (i + 1 == args.size()) {
                cerr << "black argument requires a value" << endl;
                print_strategy_indexes();
                return false;
            }
            arg_black_strategy = parse_strategy_index_arg(args[++i]);
        } else if (args[i] == "--size" || args[i] == "-s") {
            if (i + 1 == args.size()) {
                cerr << "size argument requires a number" << endl;
                return false;
            }
            arg_othello_board_size = parse_arg_othello_board_size(args[++i]);
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argv_to_args(argc, argv))) {
        return 1;
    }

    othello::game game(arg_othello_board_size);
    unique_ptr<othello::strategy> strategy_white = make_strategy_from_index(othello::white, arg_white_strategy);
    unique_ptr<othello::strategy> strategy_black = make_strategy_from_index(othello::black, arg_black_strategy);

    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;
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
