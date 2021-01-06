#ifndef OTHELLO_SCORE_FUNC_H
#define OTHELLO_SCORE_FUNC_H

#include <climits>
#include <functional>
#include <string>

#include "core.h"

namespace othello {

// The higher the positive value, the better for white player.
// The opposite applies to black-negative.
typedef std::function<int(const game &)> score_function;

int terminal_score(const game &g, piece_color player)
{
    piece_color winner = g.winner();
    if (winner == player)
        return INT_MAX;
    if (winner == opposite(player))
        return INT_MIN;
    return 0;
}

struct score_function_register {
    std::string description;
    const score_function &eval;
    int operator()(const game &g) const { return eval(g); }
};

const score_function_register pieces_diff_score = {
    "diff #pieces",
    [](const game &g){ return g.count<white>() - g.count<black>(); }
};

int pieces_diff_score_with_borders_and_corners_(const game &g, int corner_score = 6, int border_score = 2)
{
    return
        + g.count<white>(mask::inner)
        + g.count<white>(mask::border) * border_score
        + g.count<white>(mask::corners) * corner_score
        - g.count<black>(mask::inner)
        - g.count<black>(mask::border) * border_score
        - g.count<black>(mask::corners) * corner_score;
}

const score_function_register pieces_diff_score_with_borders_and_corners = {
    "#pieces with corners(6) and borders(2)",
    [](const game &g){ return pieces_diff_score_with_borders_and_corners_(g, 6, 2); }
};

}

#endif // OTHELLO_SCORE_FUNC_H
