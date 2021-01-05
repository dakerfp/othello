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

    virtual pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) = 0;

    piece_color player() const { return color; }
};


pos play_player(strategy *s, game &g) {
    assert(g.player() == s->player());
    
    std::vector<pos> possible_positions = g.possible_place_positions();
    assert(!possible_positions.empty());

    pos p = s->choose_piece_position(g, possible_positions);
    g.place_piece(p);

    return p;
}

piece_color play(game &game,
    strategy * strategy_white,
    strategy * strategy_black,
    std::function<void(const othello::game&)> showgame=nullptr,
    std::function<void(const pos&)> logpos=nullptr)
{
    if (showgame) showgame(game);
    while (1) {
        if (!game.player_can_place_any_piece(game.player())) {
            if (!game.player_can_place_any_piece(opposite(game.player())))
                break;
            game.flip_player();
        }

        pos p;
        switch (game.player()) {
        case othello::white:
            p = play_player(strategy_white, game);
            break;
        case othello::black:
            p = play_player(strategy_black, game);
            break;
        default:
            break; // ignore
        }
        if (logpos) logpos(p);
        if (showgame) showgame(game);
    }
    return game.winner();
}

bool replay(const std::vector<pos> &positions, piece_color winner) {
    game g;
    for (auto p : positions) {
        if (!g.player_can_place_any_piece(g.player()))
            g.flip_player();
        if (g.is_game_over())
            return false; // should not have ended
        g.place_piece(p);
    }
    return g.is_game_over() && winner == g.winner();
}

}

#endif // OTHELO_PLAY_H
