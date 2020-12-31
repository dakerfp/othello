
#ifndef OTHELLO_H
#define OTHELLO_H

#include <string>
#include <vector>

namespace othello {

enum piece_color {
    white, black, none
};

std::string to_string(piece_color c) {
    switch (c)
    {
    case white: return "white";
    case black: return "black";
    case none: return "none";
    default:
        return "??";
    }
}

piece_color opposite(piece_color c)
{
    switch (c)
    {
    case white:
        return black;
    case black:
        return white;
    default:
        return c;
    }
}

struct pos {
    int x, y;
};

namespace directions {
enum direction {
    N, S, E, W
};

direction all[4] = {N, S, E, W};
}

using namespace directions;

pos next_pos(const pos &p, direction d) {
    switch (d)
    {
    case N: return {p.x, p.y - 1};
    case S: return {p.x, p.y + 1};
    case W: return {p.x - 1, p.y};
    case E: return {p.x + 1, p.y};
    default:
        return p;        
    }
}

class game {
    int size;
    std::vector<piece_color> board;
    piece_color next_player;

    piece_color &get(const pos &p)
    {
        return board[p.y * size + p.x];
    }

    piece_color get(const pos &p) const
    {
        return board[p.y * size + p.x];
    }

    void flip_pieces_in_direction(piece_color player, const pos &p, direction d)
    {
        if (!can_piece_surround_in_direction(player, p, d))
            return;

        // flip pieces
        for (pos np = next_pos(p, d);
            is_position_valid(np) && get(np) == opposite(player);
            np = next_pos(np, d)) {
            get(np) = player;
        }
    }

    bool can_piece_surround_in_direction(piece_color player, const pos &p, direction d) const
    {
        pos np = next_pos(p, d);
        if (!is_position_valid(np))
            return false;

        if (opposite(player) != get(np))
            return false;

        int count = 0;
        while (is_position_valid(np)) {
            if (get(np) == none) {
                return false;
            } else if (player == get(np)) {
                return count > 0;
            } else {
                count++;
                np = next_pos(np, d);
            }
        }
        return false;
    }

public:
    game(int s=8)
        : size(s), board(s * s)
    {
        init();
    }

    game(const game& g)
        : size(g.size), board(g.board), next_player(g.next_player)
    {
    }

    piece_color player() const { return next_player; }

    void init()
    {
        for (int y = 0; y < size; y++)
            for (int x = 0; x < size; x++)
                get({x,y}) = none;
        
        // populate middle pieces
        get({size/2-1,size/2-1}) = white;
        get({size/2,size/2}) = white;
        get({size/2,size/2-1}) = black;
        get({size/2-1,size/2}) = black;

        next_player = white;
    }

    int get_size() const { return size; }

    piece_color operator[](const pos &p) const
    {
        return board[p.y * size + p.x];
    }

    bool is_position_valid(const pos &p) const
    {
        return p.x >= 0 && p.x < size && p.y >= 0 && p.y < size;
    }

    bool can_play(const pos &p, piece_color player_=none) const
    {
        if (!is_position_valid(p))
            return false;
        
        if (get(p) != none)
            return false;
        
        if (player_ == none)
            player_ = player();

        for (direction d : directions::all) {
            if (can_piece_surround_in_direction(player_, p, d))
                return true;
        }

        return false;
    }

    bool place_piece(const pos &p)
    {
        if (!can_play(p))
            return false;
        
        for (direction d : directions::all)
            flip_pieces_in_direction(player(), p, d);

        get(p) = player();

        flip_player();

        return true;
    }

    std::vector<pos> possible_place_positions() const
    {
        std::vector<pos> possible_positions;
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                pos p = {x, y};
                if (can_play(p))
                    possible_positions.push_back(p);
            }
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
                if (get({x,y})== pc)
                    count++;
        return count;
    }

    piece_color winner() const {
        int pw = count_pieces(white);
        int pb = count_pieces(black);
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

class strategy
{
private:
    piece_color color;

protected:
    virtual pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) = 0;

public:
    static constexpr const char * description = "";

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

#endif // OTHELLO_H
