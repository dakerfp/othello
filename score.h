#ifndef OTHELLO_SCORE_FUNC_H
#define OTHELLO_SCORE_FUNC_H

#include <climits>
#include <functional>
#include <string>

#include "core.h"

namespace othello::score {

// The higher the positive value, the better for white player.
// The opposite applies to black - negative.
typedef std::function<int(const game &)> function;

int terminal(const game &g, piece_color player)
{
    piece_color winner = g.winner();
    if (winner == player)
        return INT_MAX;
    if (winner == opposite(player))
        return INT_MIN;
    return 0;
}

int pieces_diff_with_borders_and_corners_(const game &g, int corner_score = 6, int border_score = 2)
{
    return
        + g.count<white>(mask::inner)
        + g.count<white>(mask::border) * border_score
        + g.count<white>(mask::corners) * corner_score
        - g.count<black>(mask::inner)
        - g.count<black>(mask::border) * border_score
        - g.count<black>(mask::corners) * corner_score;
}

int possible_place_positions_(const game &g, int corner_score = 6)
{
    int cur_player_score =
        + ((g.player() == white)
            ? popcount(g.possible_place_positions().bitmap)
            : -popcount(g.possible_place_positions().bitmap))
        + g.count<white>(mask::corners) * corner_score
        - g.count<black>(mask::corners) * corner_score;

    return cur_player_score;
}

int pieces_diff_score(const game &g)
{
    return g.count<white>() - g.count<black>();
}

int pieces_diff_with_borders_and_corners(const game &g)
{
    return pieces_diff_with_borders_and_corners_(g, 6, 2);
}

int possible_place_positions(const game &g)
{
    return possible_place_positions_(g, 6);
}

}

#endif // OTHELLO_SCORE_FUNC_H
