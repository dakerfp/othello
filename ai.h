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

strategy maximize_score_strategy(const score_function_register &score=pieces_diff_score)
{
    return [&score](const game &o, piece_color player, positions possible_positions)
    {
        int max_score = INT_MIN;
        bitpos max_p = 0;
        for (bitpos p : possible_positions) {
            game g = o.test_piece(p);
            int current_score = (player == white) ? score(g) : -score(g);
            if (current_score > max_score) {
                max_score = current_score;
                max_p = p;
            }
        }
        return max_p;
    };
}

int minmax_score_game_state(const game &g, piece_color player, int depth, score_function_register score)
{
    if (g.is_game_over())
        return terminal_score(g, player);

    if (depth <= 0)
        return (player == white) ? score(g) : -score(g);

    bool maximize = g.player() == player;
    int final_score = maximize ? INT_MIN : INT_MAX;
    auto possible_places = g.possible_place_positions();
    for (bitpos p : possible_places) {
        int current_score = minmax_score_game_state(g.test_piece(p), player, depth - 1, score);
        if (maximize)
            final_score = std::max(final_score, current_score);
        else
            final_score = std::min(final_score, current_score);
    }
    return final_score;
}

strategy minmax_strategy(int max_depth, const score_function_register &score=pieces_diff_score)
{
    return [&score, max_depth](const game &g, piece_color player, positions possible_positions)
    {
        int max_score = INT_MIN;
        bitpos max_p = 0;
        for (bitpos p : possible_positions) {
            int current_score = minmax_score_game_state(g.test_piece(p), player, max_depth, score);
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
strategy minmax2corners = minmax_strategy(2, pieces_diff_score_with_borders_and_corners);
strategy minmax4corners = minmax_strategy(4, pieces_diff_score_with_borders_and_corners);
strategy max_liberty = maximize_score_strategy(maximize_possible_place_positions);

struct strategy_index {
    const char * description;
    strategy strat;
};

}

#endif // OTHELO_AI_H