#ifndef OTHELO_PLAY_H
#define OTHELO_PLAY_H

#include "core.h"

#include <vector>

namespace othello {

using strategy = std::function<bitpos(const game &, piece_color, positions)>;

bitpos play_player(strategy strat, piece_color player, game &g) {
    assert(g.player() == player);
    
    auto possible_positions = g.possible_place_positions();
    assert(possible_positions.size() != 0);

    bitpos p = strat(g, player, possible_positions);
    g.place_piece(p);

    return p;
}

piece_color play(game &game,
    strategy strategy_black,
    strategy strategy_white,
    std::function<void(const othello::game&, const othello::pos&)> showgame=nullptr,
    std::function<void(const pos&)> logpos=nullptr)
{
    pos p = {-1, -1};
    while (!game.is_game_over()) {
        if (showgame) showgame(game, p);
        switch (game.player()) {
        case othello::black:
            p = pos::from_bitpos(play_player(strategy_black, black, game));
            break;
        case othello::white:
            p = pos::from_bitpos(play_player(strategy_white, white, game));
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
