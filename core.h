
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

    void flip_pieces_in_direction(piece_color player, const pos &p, direction d)
    {
        if (!can_piece_surround_in_direction(player, p, d))
            return;

        // flip pieces
        for (pos np = next_pos(p, d);
            is_position_valid(np) && board.get(np) == opposite(player);
            np = next_pos(np, d)) {
            board.set(np, player);
        }
    }

    bool can_piece_surround_in_direction(piece_color player, const pos &p, direction d) const
    {
        pos np = next_pos(p, d);
        if (!is_position_valid(np))
            return false;

        if (opposite(player) != board.get(np))
            return false;

        int count = 0;
        while (is_position_valid(np)) {
            if (board.get(np) == none) {
                return false;
            } else if (player == board.get(np)) {
                return count > 0;
            } else {
                count++;
                np = next_pos(np, d);
            }
        }
        return false;
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

    piece_color player() const { return next_player; }

    void init()
    {
        board.reset();
        
        // populate middle pieces
        board.set({3,3}, white);
        board.set({4,4}, white);
        board.set({3,4}, black);
        board.set({4,3}, black);

        next_player = white;
    }

    piece_color operator[](const pos &p) const
    {
        return board.get(p);
    }

    constexpr bool is_position_valid(const pos &p) const
    {
        return p.x >= 0 && p.x < size && p.y >= 0 && p.y < size;
    }

    constexpr bool is_index_valid(index i) const
    {
        return i < sizeof(uint64) * 8;
    }

    bool unchecked_can_play(bitpos p, piece_color player_) const
    {
        if (board.get(p) != none)
            return false;

        for (direction d : directions::all) {
            if (can_piece_surround_in_direction(player_, pos::from_bitpos(p), d))
                return true;
        }

        return false;
    }

    bool unchecked_can_play(const pos &p, piece_color player_) const
    {
        if (board.get(p) != none)
            return false;

        for (direction d : directions::all) {
            if (can_piece_surround_in_direction(player_, p, d))
                return true;
        }

        return false;
    }

    bool can_play(const pos &p, piece_color player_) const
    {
        if (!is_position_valid(p))
            return false;
        
        return unchecked_can_play(p, player_);
    }

    bool unchecked_place_piece(const pos &p)
    {
        for (direction d : directions::all)
            flip_pieces_in_direction(player(), p, d);

        board.set(p, player());

        flip_player();

        return true;
    }

    bool place_piece(const pos &p)
    {
        if (!can_play(p, player()))
            return false;

        return unchecked_place_piece(p);
    }

    game test_piece(const pos &p) const
    {
        game g(*this);
        g.unchecked_place_piece(p);
        return g;
    }

    std::vector<pos> possible_place_positions() const
    {
        std::vector<pos> possible_positions;
        for (bitpos bp : positions::all()) {
            pos p = pos::from_bitpos(bp);
            if (can_play(p, player()))
                possible_positions.push_back(p);
        }
        return possible_positions;
    }

    bool player_can_place_any_piece(piece_color pc) const
    {
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                pos p = {x, y};
                if (can_play(p, pc))
                    return true;
            }
        }
        return false;
    }

    bool is_game_over() const
    {
        return !player_can_place_any_piece(player()) &&
            !player_can_place_any_piece(opposite(player()));
    }

    int count_pieces(piece_color pc) const {
        int count = 0;
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                if (board.get({x,y})== pc)
                    count++;
        return count;
    }

    constexpr int count_whites(uint64 mask=mask::all) const {
        return board.count_whites(mask);
    }

    constexpr int count_blacks(uint64 mask=mask::all) const {
        return board.count_blacks(mask);
    }

    piece_color winner() const {
        int pw = count_whites();
        int pb = count_blacks();
        if (pb == pw)
            return none;
        return pw > pb ? white : black;
    }

    void flip_player() {
        next_player = opposite(next_player);
    }

    bool is_border_x(pos p) const {
        return p.x == 0 || p.x == size - 1;
    }

    bool is_border_y(pos p) const {
        return p.y == 0 || p.y == size - 1;
    }

    bool is_corner(pos p) const {
        return is_border_x(p) && is_border_y(p);
    }

    bool is_border(pos p) const {
        return is_border_x(p) || is_border_y(p);
    }
};

}

#endif // OTHELLO_CORE_H
