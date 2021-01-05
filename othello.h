
#ifndef OTHELLO_H
#define OTHELLO_H

#include <cassert>
#include <string>
#include <vector>
#include <numeric>
#include <functional>

#if !defined(popcount)
#define popcount(x) __builtin_popcount(x)
#endif

namespace othello {

enum piece_color {
    none = 0,
    white = 1 << 0,
    black = 1 << 1
};

int sign(piece_color pc) {
    switch (pc)
    {
    case white: return 1;
    case black: return -1;
    default:
        return 0;
    }
}

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

typedef unsigned long long int uint64;
typedef uint64 index;

struct pos {
    int x, y;
    bool operator==(const pos &o) const { return x == o.x && y == o.y; }
    index to_index() const { return y * 8 + x; }
    static pos from_index(index i) { return {int(i) % 8, int(i) / 8}; }
};

namespace directions {
enum direction {
    N = -8,
    S = +8,
    E = +1,
    W = -1
};

direction all[4] = {N, S, E, W};
}

using namespace directions;

constexpr pos next_pos(const pos &p, direction d) {
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

constexpr index next_index(index i, direction d) {
    return i + index(d);
}

namespace util {
    static constexpr int size = 8;
    static constexpr int last = 7;

    static constexpr index index_from_pos(const pos &p)
    {
        return p.y * size + p.x;
    }

    static constexpr uint64 bit(const pos &p)
    {
        return uint64(1) << index_from_pos(p);
    }
};

class board8x8 {
private:
    uint64 whites;
    uint64 blacks;

public:
    board8x8()
        : whites(0), blacks(0)
    {}

    board8x8(const board8x8 &b)
        : whites(b.whites), blacks(b.blacks)
    {}

    void reset()
    {
        whites = blacks = 0;
    }

    constexpr piece_color get(index i) const
    {
        uint64 white_bit = (whites >> i) & 0x01;
        uint64 black_bit = (blacks >> i) & 0x01;
        return piece_color(white_bit | (black_bit << 1));
    }

    constexpr piece_color get(const pos &p) const
    {
        int index = util::index_from_pos(p);
        return get(index);
    }

    constexpr void set_white(index i)
    {
        whites |= uint64(1) << i;
        blacks &= ~(uint64(1) << i);
    }

    constexpr void set_black(index i)
    {
        blacks |= uint64(1) << i;
        whites &= ~(uint64(1) << i);
    }

    constexpr void set(index i, piece_color c)
    {
        if (c == white) {
            set_white(i);
        } else { // XXX ignore none
            set_black(i);
        }
    }

    constexpr void set(const pos &p, piece_color c)
    {
        set(util::index_from_pos(p), c);
    }

    constexpr int count_whites(uint64 mask=~0) const {
        return popcount(whites & mask);
    }

    constexpr int count_blacks(uint64 mask=~0) const {
        return popcount(blacks & mask);
    }

    constexpr uint64 nones() const {
        return ~(whites | blacks);
    }

    constexpr uint64 count_nones() const {
        return popcount(nones());
    }

    static constexpr uint64 corners_mask = util::bit({0, 0})
        | util::bit({0, util::last})
        | util::bit({util::last, 0})
        | util::bit({util::last, util::last});

    static constexpr uint64 border_mask() {
        uint64 mask = 0;
        for (int i = 1; i < util::size; i++) {
            mask |= util::bit({0, i});
            mask |= util::bit({util::last, i});
            mask |= util::bit({i, 0});
            mask |= util::bit({i, util::last});
        }
        return mask;
    }

    int count(piece_color pc) const {
        switch (pc) {
        case white: return count_whites();
        case black: return count_blacks();
        case none: return count_nones();
        default: return 0;
        }
    }
};

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
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                pos p = {x, y};
                if (can_play(p, player()))
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
                if (board.get({x,y})== pc)
                    count++;
        return count;
    }

    constexpr int count_whites(uint64 mask=~0) const {
        return board.count_whites(mask);
    }

    constexpr int count_blacks(uint64 mask=~0) const {
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

}

#endif // OTHELLO_H
