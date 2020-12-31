#ifndef OTHELO_AI_H
#define OTHELO_AI_H

#include "othello.h"

#include <cstdlib>
#include <memory>

namespace othello {


class random_strategy : public strategy
{
public:
    random_strategy(piece_color color=none)
        : strategy(color)
    {}

    pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) override
    {
        int rand_pos = rand() % possible_positions.size();
        return possible_positions[rand_pos];
    }
};

class random_strategy_with_borders_first : public random_strategy
{
public:
    random_strategy_with_borders_first(piece_color color=none)
        : random_strategy(color)
    {}

    pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) override
    {
        // check for corners first
        for (pos p : possible_positions)
            if (g.is_corner(p))
                return p;

        // check for borders first
        for (pos p : possible_positions)
            if (g.is_border(p))
                return p;

        // otherwise go random
        return random_strategy::choose_piece_position(g, possible_positions);
    }
};

}

#endif // OTHELO_AI_H