#ifndef OTHELO_AI_H
#define OTHELO_AI_H

#include "othello.h"

#include <cstdlib>
#include <memory>

namespace othello {

class strategy
{
private:
    piece_color color;

protected:
    virtual pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) = 0;

public:
    strategy(piece_color color_=none)
        : color(color_)
    {}
    virtual void reset(piece_color color_) { color = color_; }
    piece_color player() const { return color; }
    bool play(game &g) {
        if (g.player() != player())
            return false;
        
        std::vector<pos> possible_positions = g.possible_place_positions();
        if (possible_positions.empty())
            return false;

        pos p = choose_piece_position(g, possible_positions);
        g.place_piece(p);

        return true;
    }
};

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

piece_color play(game &game,
    const std::unique_ptr<strategy> &strategy_white,
    const std::unique_ptr<strategy> &strategy_black)
{
    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;
            game.flip_player();
        }

        switch (game.player()) {
        case othello::white:
            strategy_white->play(game);
            break;
        case othello::black:
            strategy_black->play(game);
            break;
        }
    }
    return game.winner();
}

}

#endif // OTHELO_AI_H