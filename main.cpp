#include <algorithm>
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

char alpha_from_index(int index)
{
    return 'a' + index;
}

int index_from_alpha(char alpha)
{
    return alpha - 'a';
}

int index_from_digit(char digit)
{
    return digit - '0';
}

bool parse_pos(string s, othello::pos &pos)
{
    cout << s << endl;
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    cout << s << endl;

    if (s.size() != 2)
        return false;

    if (std::isalpha(s[0]) && std::isdigit(s[1])) {
        pos.x = index_from_alpha(s[0]);
        pos.y = index_from_digit(s[1]);
        return true;
    }
    if (std::isalpha(s[1]) && std::isdigit(s[0])) {
        pos.x = index_from_alpha(s[1]);
        pos.y = index_from_digit(s[0]);
        return true;
    }
    return false;
}

void print_othello_board(const othello::game &board, othello::piece_color pc=othello::none)
{
    cout << "  ";
    for (int x = 0; x < board.get_size(); x++)
        cout << alpha_from_index(x);
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
            cout << "[" << to_symbol(game.player()) << "] play position:" << endl;

            string s;
            getline(cin, s);
            if (!parse_pos(s, p))
                continue;

            if (game.can_play(p))
                break;
        }
        return p;
    }
};

std::unique_ptr<othello::strategy> make_strategy_from_index(othello::piece_color color, unsigned int index)
{
    switch (index) {
    case 0:
        return move(make_unique<human_strategy>(color));
    case 1:
        return move(make_unique<othello::random_strategy>(color));
    case 2:
        return move(make_unique<othello::random_strategy_with_borders_first>(color));
    case 3:
        return move(make_unique<othello::random_strategy_with_corners_and_borders_first>(color));
    default:
        return nullptr;
    }
}

template<int index, typename T>
void print_strategy_index()
{
    cout << index << " - " << T::description << endl;
}

void print_strategy_indexes()
{
    cout << "select strategy for player:" << endl;
    print_strategy_index<0, human_strategy>();
    print_strategy_index<1, othello::random_strategy>();
    print_strategy_index<2, othello::random_strategy_with_borders_first>();
    print_strategy_index<3, othello::random_strategy_with_corners_and_borders_first>();
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
    if (strategy > 3)
        strategy = 0;
    return strategy;
}

unsigned int arg_white_strategy = 0;
unsigned int arg_black_strategy = 0;

bool parse_args(vector<string> args)
{
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "help" || args[i] == "--help" || args[i] == "-h") {
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
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argv_to_args(argc, argv))) {
        return 1;
    }

    othello::game game;
    unique_ptr<othello::strategy> strategy_white = make_strategy_from_index(othello::white, arg_white_strategy);
    unique_ptr<othello::strategy> strategy_black = make_strategy_from_index(othello::black, arg_black_strategy);

    othello::piece_color winner = othello::play(game, strategy_white, strategy_black);
    print_othello_board(game);

    cout << endl << game_winner_message(winner) << endl;
}
