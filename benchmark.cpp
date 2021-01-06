#include "othello.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace othello;

void benchmark(unsigned repeat)
{
    random_strategy random;
    random_strategy_with_borders_first random_with_borders_first;
    random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    maximize_score_strategy max_pieces;
    maximize_score_strategy max_liberty(none, maximize_possible_place_positions);
    minmax_strategy minmax2(none, 2);
    minmax_strategy minmax4(none, 4);
    minmax_strategy minmax2corners(none, 2, pieces_diff_score_with_borders_and_corners);
    minmax_strategy minmax4corners(none, 4, pieces_diff_score_with_borders_and_corners);

    vector<strategy *> strategies = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &max_liberty,
        &minmax2
        // &minmax4,
        // &minmax2corners,
        // &minmax4corners
    };

    auto winmatrix = winrate_matrix(strategies, repeat);
    auto acc_scores = accumulate_score(winmatrix);

    for (unsigned i = 0; i < winmatrix.size(); i++) {
        for (unsigned j = 0; j < winmatrix.size(); j++) {
            cout << strategies[i]->description()
                << " vs "
                << strategies[j]->description()
                << ": "
                << winmatrix[i][j]
                << endl;
        }
        cout << "------------------------\n";
    }

    cout << "acccumulated scores:\n";
    for (unsigned i = 0; i < strategies.size(); i++) {
        cout << '\t' << acc_scores[i] << "\t - " << strategies[i]->description() << endl;
    }
}

int main(int argc, const char * argv[]) {
    unsigned repeat = (argc == 2) ? strtoul(argv[1], 0, 10) : 1000;
    auto start = chrono::high_resolution_clock::now();
    benchmark(repeat);
    auto finish = chrono::high_resolution_clock::now();
    cout << "elapsed time: " << (finish - start).count() << endl;
}