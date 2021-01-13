#ifndef OTHELO_AI_H
#define OTHELO_AI_H

#include <algorithm>
#include <cstdlib>
#include <climits>
#include <memory>

#include "core.h"
#include "score.h"

namespace othello::strat {

bitpos random_strategy(const game &g, piece_color player, positions possible_positions)
{
    int rand_pos = rand() % possible_positions.size();
    for (bitpos p : possible_positions)
        if (rand_pos-- == 0)
            return p;
    return 0; // XXX test
}

bitpos random_strategy_with_borders_first(const game &g, piece_color player, positions possible_positions)
{
    // check for borders first
    for (bitpos p : possible_positions)
        if (p & mask::border)
            return p;

    // otherwise go random
    return random_strategy(g, player, possible_positions);
}

bitpos random_strategy_with_corners_and_borders_first(const game &g, piece_color player, positions possible_positions)
{
    // check for corners first
    for (bitpos p : possible_positions)
        if (p & mask::corners)
            return p;

    // otherwise go with borders first
    return random_strategy_with_borders_first(g, player, possible_positions);
}

strategy maximize_score_strategy(score::function scoref=score::pieces_diff_score)
{
    return [scoref](const game &o, piece_color player, positions possible_positions)
    {
        int max_score = INT_MIN;
        bitpos max_p = 0;
        for (bitpos p : possible_positions) {
            game g = o.test_piece(p);
            int current_score = (player == white) ? scoref(g) : -scoref(g);
            if (current_score > max_score) {
                max_score = current_score;
                max_p = p;
            }
        }
        return max_p;
    };
}

strategy minmax_strategy(int max_depth, score::function scoref=score::pieces_diff_score)
{
    return [scoref, max_depth](const game &g, piece_color player, positions possible_positions)
    {
        int max_score = INT_MIN;
        bitpos max_p = 0;
        for (bitpos p : possible_positions) {
            int state_score = score::minmax_score_game_state(g.test_piece(p), max_depth, scoref);
            int current_score = (player == white) ? state_score : -state_score;
            if (current_score >= max_score) {
                max_score = current_score;
                max_p = p;
            }
        }
        return max_p;
    };
}

strategy max_pieces = maximize_score_strategy();
strategy minmax2 = minmax_strategy(2);
strategy minmax4 = minmax_strategy(4);
strategy minmax8 = minmax_strategy(8);
strategy minmax2corners = minmax_strategy(2, score::pieces_diff_with_borders_and_corners);
strategy minmax4corners = minmax_strategy(4, score::pieces_diff_with_borders_and_corners);
strategy max_liberty = maximize_score_strategy(score::possible_place_positions);

template<int steps=8>
bitpos start_random(const game &g, piece_color player, positions possible_positions)
{
    if (g.count<any>() <= steps)
        return random_strategy(g, player, possible_positions);
    else
        return minmax4(g, player, possible_positions);
}

struct strategy_index {
    const char * description;
    strategy strat;
};

}

#endif // OTHELO_AI_H