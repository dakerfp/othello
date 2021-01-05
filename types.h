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

int sign(piece_color pc) {
    switch (pc)
    {
    case white: return 1;
    case black: return -1;
    default:
        return 0;
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
typedef uint64 bitpos;
typedef uint64 bitmap8x8;

struct pos {
    int x, y;
    bool operator==(const pos &o) const { return x == o.x && y == o.y; }
    index to_index() const { return y * 8 + x; }
    bitpos to_bitpos() const { return 1 << to_index(); }
    static pos from_index(index i) { return {int(i) % 8, int(i) / 8}; }
    static pos from_bitpos(bitpos b) { return from_index(first_bit_index(b)); }
};

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
}

struct indexes {
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

    constexpr static indexes all() {
        return {~uint64(0)};
    }
};

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

constexpr pos next_pos(const pos &p, direction d) {
    switch (d)
    {
    case N: return {p.x, p.y - 1};
    case S: return {p.x, p.y + 1};
    case W: return {p.x - 1, p.y};
    case E: return {p.x + 1, p.y};
    case NW: return {p.x - 1, p.y - 1};
    case NE: return {p.x + 1, p.y - 1};
    case SW: return {p.x - 1, p.y + 1};
    case SE: return {p.x + 1, p.y + 1};
    default:
        return p;
    }
}

constexpr bitpos next_bitpos(bitpos b, direction d) {
    switch (d)
    {
    case N: return b << 8;
    case S: return b >> 8;
    case W: return b << 1;
    case E: return b >> 1;
    case NW: return b << 9;
    case NE: return b << 7;
    case SW: return b >> 7;
    case SE: return b >> 9;
    default:
        return b;
    }
}

namespace mask {
    static constexpr bitmap8x8 make_border() {
        uint64 mask = 0;
        for (int i = 1; i < util::size; i++) {
            mask |= util::bit({0, i});
            mask |= util::bit({util::last, i});
            mask |= util::bit({i, 0});
            mask |= util::bit({i, util::last});
        }
        return mask;
    }

    static constexpr bitmap8x8 all = ~0;
    static constexpr bitmap8x8 corners = util::bit({0, 0})
        | util::bit({0, util::last})
        | util::bit({util::last, 0})
        | util::bit({util::last, util::last});
    static constexpr bitmap8x8 border = make_border();
    static constexpr bitmap8x8 inner = all ^ border;
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
        whites |= util::bit(i);
        blacks &= ~util::bit(i);
    }

    constexpr void set_black(index i)
    {
        blacks |= util::bit(i);
        whites &= ~util::bit(i);
    }

    constexpr void set_none(index i)
    {
        whites &= ~util::bit(i);
        blacks &= ~util::bit(i);
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

    constexpr int count_whites(uint64 mask=mask::all) const {
        return popcount(whites & mask);
    }

    constexpr int count_blacks(uint64 mask=mask::all) const {
        return popcount(blacks & mask);
    }

    constexpr uint64 nones() const {
        return ~(whites | blacks);
    }

    constexpr uint64 count_nones() const {
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
