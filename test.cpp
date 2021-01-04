#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"
#include "benchmark.h"
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

void benchmark_winrate()
{
    othello::random_strategy random;
    othello::random_strategy_with_borders_first random_with_borders_first;
    othello::random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    othello::maximize_score_strategy max_pieces;
    othello::minmax_strategy minmax2(othello::none, 2);
    othello::minmax_strategy minmax4(othello::none, 4);
    othello::minmax_strategy minmax2corners(othello::none, 2, othello::pieces_diff_score_with_borders_and_corners);
    othello::minmax_strategy minmax4corners(othello::none, 4, othello::pieces_diff_score_with_borders_and_corners);

    vector<othello::strategy *> strategies = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &minmax2,
        &minmax4,
        &minmax2corners,
        &minmax4corners
    };
}

int main()
{
    test_initial_condition_and_first_placement();
}