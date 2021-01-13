#include "othello.h"
#include "colors.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace othello;

void benchmark_strategies(unsigned repeat, const vector<strat::strategy_index> &strategies)
{
    auto winmatrix = winrate_matrix(strategies, repeat);
    auto acc_scores = accumulate_score(winmatrix);

    // header
    for (unsigned i = 0; i < strategies.size(); i++)
        cout << "\t" << i;
    cout << endl;

    double sum_scores  = 0;
    for (auto score : acc_scores)
        sum_scores += score;
    double avg_score = sum_scores / strategies.size();

    // body
    for (unsigned i = 0; i < winmatrix.size(); i++) {
        // row
        cout << i << "\t";
        for (unsigned j = 0; j < winmatrix.size(); j++) {
            double wr = winmatrix[i][j];
            auto label_color = WHITE;
            if (fabs(wr - 0.5) <= 0.2) {
                label_color = WHITE;
            } else if (wr > 0.5) {
                label_color = GREEN;
            } else {
                label_color = RED;
            }
            cout << label_color <<  wr << "\t";
        }
        auto label_color = (acc_scores[i] > avg_score) ? GREEN : RED;
        cout << RESET << "- "
            << label_color << acc_scores[i]
            << RESET << '\t' << strategies[i].description
            << endl;
    }
    cout << "-----------------------------------------------\n";
    cout << "acccumulated scores:\n";
    for (unsigned i = 0; i < strategies.size(); i++)
        cout << '\t' << acc_scores[i] << "\t - " << strategies[i].description << endl;
}

void benchmark(unsigned repeat)
{

    vector<strat::strategy_index> strategies = {
        {"random", strat::random_strategy},
        {"border 1st", strat::random_strategy_with_borders_first},
        {"corner 1st", strat::random_strategy_with_corners_and_borders_first},
        {"max # pieces", strat::max_pieces},
        {"max liberties", strat::max_liberty},
        {"minmax 2", strat::minmax2},
        {"minmax 4", strat::minmax4}
    };

    benchmark_strategies(repeat, strategies);
}

int main(int argc, const char * argv[]) {
    unsigned repeat = (argc == 2) ? strtoul(argv[1], 0, 10) : 1000;
    auto start = chrono::high_resolution_clock::now();
    benchmark(repeat);
    auto finish = chrono::high_resolution_clock::now();
    cout << "elapsed time: " << (finish - start).count() << endl;
}