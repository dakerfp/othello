#ifndef OTHELO_AI_H
#define OTHELO_AI_H

#include "othello.h"

#include <algorithm>
#include <cstdlib>
#include <climits>
#include <memory>

namespace othello {

class random_strategy : public strategy
{
public:
    std::string description() const override { return "random"; }

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
    std::string description() const override { return "random with borders first"; }

    random_strategy_with_borders_first(piece_color color=none)
        : random_strategy(color)
    {}

    pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) override
    {
        // check for borders first
        for (pos p : possible_positions)
            if (g.is_border(p))
                return p;

        // otherwise go random
        return random_strategy::choose_piece_position(g, possible_positions);
    }
};

class random_strategy_with_corners_and_borders_first : public random_strategy_with_borders_first
{
public:
    std::string description() const override { return "random with corners and borders first"; }

    random_strategy_with_corners_and_borders_first(piece_color color=none)
        : random_strategy_with_borders_first(color)
    {}

    pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) override
    {
        // check for corners first
        for (pos p : possible_positions)
            if (g.is_corner(p))
                return p;

        // otherwise go with borders first
        return random_strategy_with_borders_first::choose_piece_position(g, possible_positions);
    }
};

// The higher the positive value, the better for white player.
// The opposite applies to black-negative.
typedef std::function<int(const game &)> score_function;

struct score_function_register {
    std::string description;
    const score_function &eval;
    int operator()(const game &g) const { return eval(g); }
};

const score_function_register pieces_diff_score = {
    "diff #pieces",
    [](const game &g){ return g.count_pieces(white) - g.count_pieces(black); }
};

int pieces_diff_score_with_borders_and_corners_(const game &g, int corner_score = 6, int border_score = 2)
{
    int score = 0;
    for (int y = 0; y < g.get_size(); y++) {
        for (int x = 0; x < g.get_size(); x++) {
            pos p = {y, x};
            if (g.is_corner(p)) {
                score += sign(g[p]) * corner_score;
            } else if (g.is_border(p)) {
                score += sign(g[p]) * border_score;
            } else {
                score += sign(g[p]);
            }
        }
    }

    return score;
}

const score_function_register pieces_diff_score_with_borders_and_corners = {
    "#pieces with corners(6) and borders(2)",
    [](const game &g){ return pieces_diff_score_with_borders_and_corners_(g, 6, 2); }
};

class maximize_score_strategy : public strategy
{
protected:
    score_function_register score_f;

    int score(const game &g) const {
        return player() == white ? score_f(g) : -score_f(g);
    }

public:
    std::string description() const override { return "maximize score (" + score_f.description + ")"; }

    maximize_score_strategy(piece_color color=none, const score_function_register &score_function=pieces_diff_score)
        : strategy(color), score_f(score_function)
    {}

    pos choose_piece_position(const game &o, const std::vector<pos> &possible_positions) override
    {
        int max_score = INT_MIN;
        pos max_p;
        for (pos p : possible_positions) {
            game g = o.test_piece(p);
            int current_score = score(g);
            if (current_score > max_score) {
                max_score = current_score;
                max_p = p;
            }
        }
        return max_p;
    }
};

class minmax_strategy : public maximize_score_strategy
{
private:
    int max_depth;

    int score_game_state(const game &o, int depth)
    {
        if (o.is_game_over()) {
            piece_color winner = o.winner();
            if (winner == player())
                return INT_MAX;
            if (winner == opposite(player()))
                return INT_MIN;
            return 0;
        }

        if (depth <= 0)
            return score(o);

        bool maximize = o.player() == player();
        int final_score = maximize ? INT_MIN : INT_MAX;
        auto possible_places = o.possible_place_positions();
        for (pos p : possible_places) {
            int score = score_game_state(o.test_piece(p), depth - 1);
            if (maximize) {
                final_score = std::max(final_score, score);
            } else {
                final_score = std::min(final_score, score);
            }
        }
        return final_score;
    }
public:
    std::string description() const override { return std::string("minmax ") + std::to_string(max_depth) + " (" + score_f.description + ")"; }

    minmax_strategy(piece_color color=none, int depth=4, const score_function_register &score_f=pieces_diff_score)
        : maximize_score_strategy(color, score_f), max_depth(depth)
    {}

    pos choose_piece_position(const game &o, const std::vector<pos> &possible_positions) override
    {
        int max_score = INT_MIN;
        pos max_p;
        for (pos p : possible_positions) {
            int score = score_game_state(o.test_piece(p), max_depth);
            if (score >= max_score) {
                max_score = score;
                max_p = p;
            }
        }
        return max_p;
    }
};

}

#endif // OTHELO_AI_H