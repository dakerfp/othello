#include "othello.h"
#include "benchmark.h"
#include "ai.h"

#include <iostream>
#include <vector>

void benchmark()
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

    auto winmatrix = othello::winrate_matrix(strategies, 2);
    auto acc_scores = othello::accumulate_score(winmatrix);

    for (int i = 0; i < winmatrix.size(); i++) {
        for (int j = 0; j < winmatrix.size(); j++) {
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
    for (int i = 0; i < strategies.size(); i++) {
        std::cout << '\t' << acc_scores[i] << "\t - " << strategies[i]->description() << std::endl;
    }
}

int main() {
    benchmark();
}