
#include <algorithm>
#include <cmath>
#include <ctime>
#include <functional>
#include <fstream>
#include <iostream>
#include <memory>

#include "othello.h"
#include "colors.h"

using namespace std;

const char * othello_billboard =
BOLDRED  " _____ _   _       _ _     \n"
BOLDRED  "|     | |_| |_ ___| | |___ \n"
BOLDBLUE "|  |  |  _|   | -_| | | . |\n"
BOLDBLUE "|_____|_| |_|_|___|_|_|___|\n"
YELLOW   " Version: " VERSION "\n" RESET;

string to_symbol(othello::piece_color pc, bool highlight=false)
{
    if (highlight)
        return pc == othello::white ? (BOLDYELLOW " X" RESET) : (BOLDCYAN " O" RESET);
    else
        return pc == othello::white ? (RED " X" RESET) : (BLUE " O" RESET);
}

string game_winner_message(othello::piece_color winner)
{
    if (winner == othello::none)
        return "draw";
    else
        return "winner is " + othello::io::to_string(winner);
}

void print_othello_board(const othello::game &game, const othello::pos &lastpos)
{
    cout << "  ";
    for (int x = 0; x < game.size; x++)
        cout << ' ' << othello::io::alpha_from_index(x);
    cout << endl;

    for (int y = 0; y < game.size; y++) {
        cout << y + 1 << " ";
        for (int x = 0; x < game.size; x++) {
            othello::pos p = {x,y};
            switch (game[p.to_bitpos()])
            {
            case othello::white:
            case othello::black:
                cout << to_symbol(game[p.to_bitpos()], p == lastpos);
                break;
            default:
                if (game.can_play(p.to_bitpos(), game.player())) {
                    cout << (GREEN " !" RESET);
                } else {
                    cout << " .";
                }
            }
        }
        cout << endl;
    }
    cout << endl;
}

void parse_command(string s, const othello::game &game) {
    if (s == "snapshot" || s == "snap") {
        cout << io::to_string(game) << endl;
    }
}

othello::bitpos human_strategy(const othello::game &game, piece_color player, othello::positions possible_positions)
{
    othello::pos p;
    while (1) {
        cout << "[" << to_symbol(player) << "] play position: ";

        string s;
        getline(cin, s);
        if (!othello::io::parse_pos(s, p)) {
            parse_command(s, game);
            continue;
        }

        if (game.can_play(p.to_bitpos(), player))
            break;
    }
    return p.to_bitpos();
}

static const vector<othello::strat::strategy_index> strategies = {
    {"human player (default)", human_strategy},
    {"random", othello::strat::random_strategy},
    {"borders first", othello::strat::random_strategy_with_borders_first},
    {"corners first", othello::strat::random_strategy_with_corners_and_borders_first},
    {"minmax 2", othello::strat::minmax2},
    {"minmax 4", othello::strat::minmax4},
    {"difficult", othello::strat::start_random},
    {"minmax 8", othello::strat::minmax8}
};

othello::strategy make_strategy_from_index(othello::piece_color color, unsigned index)
{
    if (index >= strategies.size())
        return nullptr;
    return strategies[index].strat;
}

function<void(const othello::pos &p)> make_logpos_from_filename(ofstream &file, string filename)
{
    if (filename.empty())
        return nullptr;

    file.open(filename);
    return [&file](const othello::pos &p) {
        if (!file.good()) {
            cerr << "file is not good" << endl;
            return;
        }
        file << othello::io::to_string(p) << " ";
    };
}

void print_strategy_indexes()
{
    unsigned i = 0;
    for (auto sti : strategies)
        cout << i++ << " - " << sti.description << endl;
}

void print_help()
{
    cout << "-b and -h arguments lets select the AI strategy for each player:" << endl;
    print_strategy_indexes();
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
    unsigned int index = atoi(arg.c_str());
    if (index >= strategies.size())
        index = 0;
    return index;
}

unsigned int arg_white_strategy = 0;
unsigned int arg_black_strategy = 0;
string arg_output_log_in_file = "";

bool parse_args(vector<string> args)
{
    for (unsigned i = 0; i < args.size(); i++) {
        if (args[i] == "help" || args[i] == "--help" || args[i] == "-h") {
            print_help();
            return false;
        }
        if (args[i] == "--white" || args[i] == "-w") {
            if (i + 1 == args.size()) {
                cerr << "white argument requires one of the following values:" << endl;
                print_strategy_indexes();
                return false;
            }
            arg_white_strategy = parse_strategy_index_arg(args[++i]);
        } else if (args[i] == "--black" || args[i] == "-b") {
            if (i + 1 == args.size()) {
                cerr << "black argument requires one of the following values:" << endl;
                print_strategy_indexes();
                return false;
            }
            arg_black_strategy = parse_strategy_index_arg(args[++i]);
        } else if (args[i] == "--output" || args[i] == "-o") {
            if (i + 1 == args.size()) {
                cerr << "output game log in file" << endl;
                return false;
            }
            arg_output_log_in_file = args[++i];
        } else {
            // ignore argument ?
            cerr << "unknow argument " << args[i] << endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (!parse_args(argv_to_args(argc, argv))) {
        return 1;
    }

    srand(time(nullptr));
    cout << othello_billboard << endl;

    othello::game game;
    ofstream file;
    strategy strategy_black = make_strategy_from_index(othello::black, arg_black_strategy);
    strategy strategy_white = make_strategy_from_index(othello::white, arg_white_strategy);
    function<void(const othello::pos &p)> logpos = make_logpos_from_filename(file, arg_output_log_in_file);

    auto winner = othello::play(game, strategy_black, strategy_white, &print_othello_board, logpos);
    cout << endl << game_winner_message(winner) << endl;
    file.close();
}
