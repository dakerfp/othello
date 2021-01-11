#ifndef OTHELO_PLAY_H
#define OTHELO_PLAY_H

#include "core.h"

#include <vector>

namespace othello {

class strategy
{
private:
    piece_color color;

public:
    strategy(piece_color color_=none)
        : color(color_)
    {}

    virtual std::string description() const = 0;

    virtual void reset(piece_color color_) { color = color_; }

    virtual bitpos choose_piece_position(const game &g, positions possible_positions) = 0;

    piece_color player() const { return color; }
};

bitpos play_player(strategy *s, game &g) {
    assert(g.player() == s->player());
    
    auto possible_positions = g.possible_place_positions();
    assert(possible_positions.size() != 0);

    bitpos p = s->choose_piece_position(g, possible_positions);
    g.place_piece(p);

    return p;
}

piece_color play(game &game,
    strategy * strategy_white,
    strategy * strategy_black,
    std::function<void(const othello::game&, const othello::pos&)> showgame=nullptr,
    std::function<void(const pos&)> logpos=nullptr)
{
    pos p = {-1, -1};
    while (!game.is_game_over()) {
        if (showgame) showgame(game, p);
        switch (game.player()) {
        case othello::white:
            p = pos::from_bitpos(play_player(strategy_white, game));
            break;
        case othello::black:
            p = pos::from_bitpos(play_player(strategy_black, game));
            break;
        default:
            break; // ignore
        }
        if (logpos) logpos(p);
    }
    if (showgame) showgame(game, p);

    return game.winner();
}

bool replay(const std::vector<bitpos> &recorded, piece_color expected_winner) {
    game g;
    for (auto p : recorded) {
        if (g.is_game_over())
            return false; // should not have ended
        g.place_piece(p);
    }
    return g.is_game_over() && expected_winner == g.winner();
}

}

#endif // OTHELO_PLAY_H
