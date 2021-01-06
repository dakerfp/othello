#ifndef OTHELLO_TYPES_H
#define OTHELLO_TYPES_H

#if !defined(popcount)
#define popcount(x) __builtin_popcountll(x)
#endif

#define first_bit_index(x) __builtin_ctzll(x)

namespace othello {

enum piece_color {
    none = 0,
    white = 1 << 0,
    black = 1 << 1
};

constexpr piece_color opposite(piece_color c)
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
typedef uint64 bitpos;
typedef uint64 bitmap8x8;

struct pos {
    int x, y;
    constexpr bool operator==(const pos &o) const { return x == o.x && y == o.y; }
    constexpr index to_index() const { return y * 8 + x; }
    constexpr bitpos to_bitpos() const { return bitpos(1) << to_index(); }
    constexpr static pos from_index(index i) { return {int(i) % 8, int(i) / 8}; }
    constexpr static pos from_bitpos(bitpos b) { return from_index(first_bit_index(b)); }
};

bool is_bitpos_valid(bitpos p)
{
    return popcount(p) == 1;
}

namespace util {
    static constexpr int size = 8;
    static constexpr int last = 7;

    static constexpr index index_from_pos(const pos &p)
    {
        return p.y * size + p.x;
    }

    static constexpr index to_index(bitpos b)
    {
        return first_bit_index(b);
    }

    static constexpr bitpos bit(const pos &p)
    {
        return bitpos(1) << index_from_pos(p);
    }

    static constexpr bitpos bit(index i)
    {
        return bitpos(1) << i;
    }

    static constexpr int row(bitpos p)
    {
        return to_index(p) / 8;
    }

    static constexpr int col(bitpos p)
    {
        return to_index(p) % 8;
    }
}

struct positions {
    bitmap8x8 bitmap = 0;

    constexpr int size() const { return popcount(bitmap); }
    void set_index(index i) { bitmap |= util::bit(i); }
    void set_bit(bitpos b) { bitmap |= b; }

    struct iterator {
        bitmap8x8 idx;

        bitpos operator*() const { return idx & (bitpos(1) << first_bit_index(idx)); }
        constexpr int index() const { return first_bit_index(idx); }
        void operator++() {
            idx ^= bitpos(1) << first_bit_index(idx);
        }
        bool operator==(const iterator &o) const {
            return idx == o.idx;
        }
    };

    constexpr iterator begin() const {
        return {bitmap};
    }

    constexpr iterator end() const {
        return {0};
    }

    constexpr static positions all() {
        return {~uint64(0)};
    }

    constexpr static positions none() {
        return {~uint64(0)};
    }
};

namespace mask {
    constexpr bitmap8x8 bit(int x, int y) {
        return util::bit({x, y});
    }

    static constexpr bitmap8x8 make_border() {
        bitmap8x8 mask = 0;
        for (int i = 1; i < util::size; i++) {
            mask |= bit(0, i);
            mask |= bit(util::last, i);
            mask |= bit(i, 0);
            mask |= bit(i, util::last);
        }
        return mask;
    }

    constexpr bitmap8x8 all = ~bitmap8x8(0);
    constexpr bitmap8x8 corners = bit(0, 0)
        | bit(0, util::last)
        | bit(util::last, 0)
        | bit(util::last, util::last);
    constexpr bitmap8x8 border = make_border();
    constexpr bitmap8x8 inner = all ^ border;
    constexpr bitmap8x8 north =
        bit(0, 0) | bit(1, 0) | bit(2, 0) | bit(3, 0) |
        bit(4, 0) | bit(5, 0) | bit(6, 0) | bit(6, 0);
    constexpr bitmap8x8 south =
        bit(0, util::last) | bit(1, util::last) | bit(2, util::last) | bit(3, util::last) |
        bit(4, util::last) | bit(5, util::last) | bit(6, util::last) | bit(7, util::last);
    constexpr bitmap8x8 west =
        bit(0, 0) | bit(0, 1) | bit(0, 2) | bit(0, 3) |
        bit(0, 4) | bit(0, 5) | bit(0, 6) | bit(0, 7);
    constexpr bitmap8x8 east =
        bit(util::last, 0) | bit(util::last, 1) | bit(util::last, 2) | bit(util::last, 3) |
        bit(util::last, 4) | bit(util::last, 5) | bit(util::last, 6) | bit(util::last, 7);
    constexpr bitmap8x8 nw = north | west;
    constexpr bitmap8x8 ne = north | east;
    constexpr bitmap8x8 sw = south | west;
    constexpr bitmap8x8 se = south | east;
}

namespace directions {
enum direction {
    N = -8,
    S = +8,
    E = +1,
    W = -1,
    NW = -9,
    NE = -7,
    SW = +7,
    SE = +9
};

direction all[8] = {N, S, E, W, NW, NE, SW, SE};
}

using namespace directions;

constexpr bitpos next_bitpos(bitpos b, direction d) {
    switch (d)
    {
    case N: return (b & ~mask::north) >> 8;
    case S: return (b & ~mask::south) << 8;
    case W: return (b & ~mask::west) >> 1;
    case E: return (b & ~mask::east) << 1;
    case NW: return (b & ~mask::nw) >> 9;
    case NE: return (b & ~mask::ne) >> 7;
    case SW: return (b & ~mask::sw) << 7;
    case SE: return (b & ~mask::se) << 9;
    default:
        return 0;
    }
}

class board8x8 {
private:
    bitmap8x8 whites;
    bitmap8x8 blacks;

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

    template<piece_color pc>
    constexpr bool has(bitpos b) const
    {
        if (pc == white)
            return whites & b;
        if (pc == black)
            return blacks & b;
        if (pc == none)
            return !((whites | blacks) & b);
        return (whites | blacks) & b;
    }

    constexpr piece_color get(bitpos b) const
    {
        int i = util::to_index(b);
        uint64 white_bit = (whites & b) >> i;
        uint64 black_bit = (blacks & b) >> i;
        return piece_color(white_bit | (black_bit << 1));
    }

    constexpr piece_color get(const pos &p) const
    {
        return get(p.to_bitpos());
    }

    constexpr void set_white(bitpos p)
    {
        whites |= p;
        blacks &= ~p;
    }

    constexpr void set_black(bitpos p)
    {
        blacks |= p;
        whites &= ~p;
    }

    constexpr void set_none(bitpos p)
    {
        whites &= ~p;
        blacks &= ~p;
    }

    constexpr void set(bitpos p, piece_color c)
    {
        if (c == white) {
            set_white(p);
        } else { // XXX ignore none
            set_black(p);
        }
    }

    constexpr void set(const pos &p, piece_color c)
    {
        set(util::index_from_pos(p), c);
    }

    constexpr int count_whites(bitmap8x8 mask=mask::all) const {
        return popcount(whites & mask);
    }

    constexpr int count_blacks(bitmap8x8 mask=mask::all) const {
        return popcount(blacks & mask);
    }

    constexpr bitmap8x8 nones() const {
        return ~(whites | blacks);
    }

    constexpr int count_nones() const {
        return popcount(nones());
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

}

#endif // OTHELLO_TYPES_H
