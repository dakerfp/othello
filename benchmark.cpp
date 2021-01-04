#include "othello.h"
#include "benchmark.h"
#include "ai.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>

void benchmark(unsigned repeat)
{
    othello::random_strategy random;
    othello::random_strategy_with_borders_first random_with_borders_first;
    othello::random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    othello::maximize_score_strategy max_pieces;
    othello::minmax_strategy minmax2(othello::none, 2);
    othello::minmax_strategy minmax4(othello::none, 4);
    othello::minmax_strategy minmax2corners(othello::none, 2, othello::pieces_diff_score_with_borders_and_corners);
    othello::minmax_strategy minmax4corners(othello::none, 4, othello::pieces_diff_score_with_borders_and_corners);

    std::vector<othello::strategy *> strategies = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &minmax2,
        &minmax4,
        &minmax2corners,
        &minmax4corners
    };

    auto winmatrix = othello::winrate_matrix(strategies, repeat);
    auto acc_scores = othello::accumulate_score(winmatrix);

    for (unsigned i = 0; i < winmatrix.size(); i++) {
        for (unsigned j = 0; j < winmatrix.size(); j++) {
            std::cout << strategies[i]->description()
                << " vs "
                << strategies[j]->description()
                << ": "
                << winmatrix[i][j]
                << std::endl;
        }
        std::cout << "------------------------\n";
    }

    std::cout << "acccumulated scores:\n";
    for (unsigned i = 0; i < strategies.size(); i++) {
        std::cout << '\t' << acc_scores[i] << "\t - " << strategies[i]->description() << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    unsigned repeat = (argc == 2) ? strtoul(argv[1], 0, 10) : 1000;
    auto start = std::chrono::high_resolution_clock::now();
    benchmark(repeat);
    auto finish = std::chrono::high_resolution_clock::now();
    cout << "elapsed time: " << (finish - start).count() << endl;
}