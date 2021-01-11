
#ifndef OTHELLO_CORE_H
#define OTHELLO_CORE_H

#include <cassert>
#include <string>
#include <vector>
#include <numeric>
#include <functional>

#include "types.h"

namespace othello {

class game {
    board8x8 board;
    piece_color next_player;

    template<piece_color player>
    void flip_pieces_in_direction(bitpos p, direction d)
    {
        if (!can_piece_surround_in_direction<player>(p, d))
            return;

        // flip pieces
        for (bitpos np = next_bitpos(p, d);
            is_bitpos_valid(np) && board.has<opposite(player)>(np);
            np = next_bitpos(np, d))
        {
            board.set(np, player);
        }
    }

    template<piece_color player>
    bool can_piece_surround_in_direction(bitpos p, direction d) const
    {
        int count = 0;
        for (bitpos np = next_bitpos(p, d);
            is_bitpos_valid(np);
            np = next_bitpos(np, d))
        {
            if (board.has<player>(np))
                return count > 0;

            if (!board.has<opposite(player)>(np)) // if none
                return false;

            count++;
        }
        return false;
    }

    void flip_player() {
        next_player = opposite(next_player);
    }

    bool unchecked_can_play(bitpos p, piece_color player_) const
    {
        if (player_ == white) {
            for (direction d : directions::all) {
                if (can_piece_surround_in_direction<white>(p, d))
                    return true;
            }
        } else { // black
            for (direction d : directions::all) {
                if (can_piece_surround_in_direction<black>(p, d))
                    return true;
            }
        }

        return false;
    }

    bool unchecked_place_piece(bitpos p)
    {
        if (player() == white) {
            for (direction d : directions::all)
                flip_pieces_in_direction<white>(p, d);
        } else { // black
            for (direction d : directions::all)
                flip_pieces_in_direction<black>(p, d);
        }

        board.set(p, player());

        if (player_can_place_any_piece(opposite(player())))
            flip_player();

        return true;
    }

public:
    static constexpr int size = 8;

    game()
    {
        init();
    }

    game(const game& g)
        : board(g.board), next_player(g.next_player)
    {
    }

    game &operator=(const game &o) {
        board = o.board;
        next_player = o.next_player;
        return *this;
    }

    piece_color operator[](bitpos p) const {
        return board.get(p);
    }

    piece_color player() const { return next_player; }

    void init()
    {
        board.reset();
        
        // populate middle pieces
        board.set(pos(3,3).to_bitpos(), white);
        board.set(pos(4,4).to_bitpos(), white);
        board.set(pos(3,4).to_bitpos(), black);
        board.set(pos(4,3).to_bitpos(), black);

        next_player = black;
    }

    bool can_play(bitpos p, piece_color player_) const
    {
        if (!is_bitpos_valid(p))
            return false;
        
        return unchecked_can_play(p, player_);
    }

    bool can_play(const pos &p, piece_color player_) const
    {
        return can_play(p.to_bitpos(), player_);
    }

    bool place_piece(bitpos p)
    {
        if (!can_play(p, player()))
            return false;

        return unchecked_place_piece(p);
    }

    bool place_piece(const pos &p) { return place_piece(p.to_bitpos()); }

    game test_piece(bitpos p) const
    {
        game g(*this);
        g.unchecked_place_piece(p);
        return g;
    }

    game test_piece(const pos &p) const { return test_piece(p.to_bitpos()); }

    positions possible_place_positions() const
    {
        positions possible_positions = {0};
        for (bitpos p : positions{board.nones()}) {
            if (can_play(p, player()))
                possible_positions.set_bit(p);
        }
        return possible_positions;
    }

    bool player_can_place_any_piece(piece_color pc) const
    {
        for (bitpos p : positions{board.nones()}) {
            if (can_play(p, pc))
                return true;
        }
        return false;
    }

    bool is_game_over() const
    {
        return !player_can_place_any_piece(player());
    }

    piece_color winner() const {
        int pw = count<white>();
        int pb = count<black>();
        if (pb == pw)
            return none;
        else
            return pw > pb ? white : black;
    }

    template<piece_color color>
    int count(bitmap8x8 mask=mask::all) const {
        return board.count<color>(mask);
    }

};

}

#endif // OTHELLO_CORE_H
