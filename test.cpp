#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"
#include "ai.h"

using namespace std;

void test_initial_condition_and_first_placement()
{
    othello::game g;

    assert(g.get_size() == 8);
    assert(g.player() == othello::white);

    assert(g.count_pieces(othello::white) == 2);
    assert(g.count_pieces(othello::black) == 2);
    assert(!g.is_game_over());

    assert(g.can_play({5, 3}, othello::white));
    assert(!g.can_play({5, 3}, othello::black));
    assert(!g.can_play({3, 3}, othello::white));

    g.place_piece({5, 3});

    assert(!g.can_play({5, 3}, othello::black));
    assert(!g.can_play({5, 3}, othello::white));
    assert(g.player() == othello::black);
    assert(g.count_pieces(othello::white) == 4);
    assert(g.count_pieces(othello::black) == 1);
}

double winrate(othello::strategy *a, othello::strategy *b, int n=1000)
{
    othello::game game;
    double win_score = 0;

    for (int i = 0; i < n; i++) {
        bool swap = i % 2 != 0;
        othello::strategy *strategy_white = swap ? b : a;
        othello::strategy *strategy_black = swap ? a : b;
        game.init();
        strategy_white->reset(othello::white);
        strategy_black->reset(othello::black);
        switch (play(game, strategy_white, strategy_black)) {
        case othello::white:
            win_score += swap ? 0 : 1;
            break;
        case othello::black:
            win_score += swap ? 1 : 0;
            break;
        default:
            win_score += 0.5;
        }
    }
    return win_score / n;
}

double print_winrate_score(othello::strategy *a, othello::strategy *b, int n=10000)
{
    double win = winrate(a, b, n);
    cout << a->description()
        << " vs "
        << b->description()
        << ": "
        << win
        << endl;
    return win;
}

void benchmark_winrate()
{
    othello::random_strategy random;
    othello::random_strategy_with_borders_first random_with_borders_first;
    othello::random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    othello::maximize_number_of_pieces_strategy max_pieces;
    othello::minmax_strategy<2> minmax2;
    othello::minmax_strategy<4> minmax4;

    othello::strategy* strategies[] = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &minmax2,
        &minmax4,
    };

    for (auto *strat1 : strategies) {
        bool repeated = true;
        for (auto *strat2 : strategies) {
            if (strat1 == strat2) {
                repeated = false;
                continue;
            } else if (repeated) {
                continue;
            }
            double win = print_winrate_score(strat1, strat2, 100);
        }
        cout << "------------------------" << endl;
    }
}

int main()
{
    test_initial_condition_and_first_placement();
    benchmark_winrate();
}