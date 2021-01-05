#ifndef OTHELLO_BENCHMARK_H
#define OTHELLO_BENCHMARK_H

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

#include "core.h"

using namespace std;

namespace othello {

double wincount(strategy *strategy_white, strategy *strategy_black, unsigned n)
{
    game game;
    double wins = 0;

    for (unsigned i = 0; i < n; i++) {
        game.init();
        strategy_white->reset(white);
        strategy_black->reset(black);
        switch (play(game, strategy_white, strategy_black)) {
        case white:
            wins += 1;
            break;
        case black:
            // win_score += 0;
            break;
        default:
            wins += 0.5;
        }
    }
    return wins;
}

double winrate(strategy *a, strategy *b, unsigned n)
{
    assert(n % 2 == 0);

    double win_as_whites = wincount(a, b, n / 2);
    double win_as_blacks = n / 2 - wincount(b, a, n / 2);
    return (win_as_whites + win_as_blacks) / n;
}

bool better_than(strategy *a, strategy *b, double eps=0, unsigned n=100)
{
    return winrate(a, b, n) >= 0.5 - eps;
}

std::vector<std::vector<double>> winrate_matrix(const std::vector<strategy *> &strategies, unsigned repeat=100)
{
    unsigned N = strategies.size();

    std::vector<std::vector<double>> winrate_matrix(N);
    for (unsigned i = 0; i < N; i++) {
        winrate_matrix[i] = std::vector<double>(N);
        winrate_matrix[i][i] = 0.5;
    }

    // build an anti 1 - x symmetric matrix
    for (unsigned i = 0; i < N; i++) {
        for (unsigned j = i + 1; j < N; j++) {
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
    for (unsigned i = 0; i < scores.size(); i++) {
        const vector<double> &row = winmatrix[i];
        for (auto v : row)
            scores[i] += v;
    }
    return scores;
}

}

#endif