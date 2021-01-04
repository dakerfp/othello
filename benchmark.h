#ifndef OTHELLO_BENCHMARK_H
#define OTHELLO_BENCHMARK_H

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include "othello.h"

using namespace std;

namespace othello {

double winrate(strategy *a, strategy *b, unsigned n)
{
    game game;
    double win_score = 0;

    assert(n % 2 == 0);

    for (int i = 0; i < n; i++) {
        bool swap = i % 2 != 0;
        strategy *strategy_white = swap ? b : a;
        strategy *strategy_black = swap ? a : b;
        game.init();
        strategy_white->reset(white);
        strategy_black->reset(black);
        switch (play(game, strategy_white, strategy_black)) {
        case white:
            win_score += swap ? 0 : 1;
            break;
        case black:
            win_score += swap ? 1 : 0;
            break;
        default:
            win_score += 0.5;
        }
    }
    return win_score / n;
}

std::vector<std::vector<double>> winrate_matrix(const std::vector<strategy *> &strategies, unsigned repeat=100)
{
    int N = strategies.size();

    std::vector<std::vector<double>> winrate_matrix(N);
    for (int i = 0; i < N; i++) {
        winrate_matrix[i] = std::vector<double>(N);
        winrate_matrix[i][i] = 0.5;
    }

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double win = winrate(strategies[i], strategies[j], repeat);
            winrate_matrix[i][j] = win;
            winrate_matrix[j][i] = 1.0 - win;
        }
    }

    return winrate_matrix;
}

vector<double> accumulate_score(const std::vector<std::vector<double>> &winmatrix)
{
    vector<double> scores(winmatrix.size());
    for (int i = 0; i < scores.size(); i++) {
        const vector<double> &row = winmatrix[i];
        for (auto v : row)
            scores[i] += v;
    }
    return scores;
}

}

#endif