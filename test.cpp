
#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"

using namespace std;
using namespace othello;

void test_positions()
{
    bitmap8x8 bmp = util::bit(42);
    positions poss = {bmp};
    assert(bmp == uint64(1) << 42);
    assert(poss.size() == 1);

    bmp = 0;
    poss = {bmp};
    assert(poss.size() == 0);
    assert(poss.begin() == poss.end());

    bmp = util::bit(7) | util::bit(42);
    poss = {bmp};
    assert(poss.size() == 2);

    auto it = poss.begin();
    assert(*it == util::bit(7));
    ++it;
    assert(it.index() == 42);
    assert(*it == util::bit(42));
    ++it;
    assert(*it == 0);
    assert(it == poss.end());

    int count = 0;
    for (bitpos p : poss) {
        assert(p != 0);
        count++;
        if (count > 10)
            assert(0);
    }
    assert(count == 2);
}

bool next_bitpos_is_valid(bitpos b, direction d)
{
    auto np = next_bitpos(b, d);
    return is_bitpos_valid(np);
}

void test_bitpos_direction()
{
    pos p = {3,3};
    bitpos bp = p.to_bitpos();

    for (auto d : directions::all)
        assert(next_bitpos_is_valid(bp, d));

    p = {0,0};
    bp = p.to_bitpos();
    assert(!next_bitpos_is_valid(bp, directions::N));
    assert(!next_bitpos_is_valid(bp, directions::NW));
    assert(!next_bitpos_is_valid(bp, directions::NE));
    assert(!next_bitpos_is_valid(bp, directions::W));
    assert(!next_bitpos_is_valid(bp, directions::SW));
    assert(next_bitpos_is_valid(bp, directions::S));
    assert(next_bitpos_is_valid(bp, directions::SE));
    assert(next_bitpos_is_valid(bp, directions::E));
}

void test_initial_condition_and_first_placement()
{
    game g;

    assert(g.size == 8);
    assert(g.player() == black);

    assert(g.count<white>() == 2);
    assert(g.count<black>() == 2);
    assert(g.count<none>() == 8 * 8 - 4);
    assert(!g.is_game_over());

    assert(g.can_play({5, 4}, black));
    assert(!g.can_play({5, 4}, white));
    assert(!g.can_play({3, 4}, black));

    g.place_piece({5, 4});

    assert(!g.can_play({5, 4}, white));
    assert(!g.can_play({5, 4}, black));
    assert(g.player() == white);
    assert(g.count<white>() == 1);
    assert(g.count<black>() == 4);
    assert(g.count<none>() == 8 * 8 - 5);
}

void test_parse_game_positions()
{
    assert(io::to_string({0,0}) == "a1");
    assert(io::to_string({7,7}) == "h8");

    auto replay = io::parse_game_positions("a1 b2 c3 d4 e5 f6 g7 h8");
    assert(replay.size() == 8);
    int i = 0;
    for (bitpos p : replay) {
        pos pt = {i, i};
        assert(p == pt.to_bitpos());
        i++;
    }

    replay = io::parse_game_positions("1a 2b 3c 4d 5e 6f 7g 8h");
    assert(replay.size() == 8);
    i = 0;
    for (bitpos p : replay) {
        pos pt = {i, i};
        assert(p == pt.to_bitpos());
        i++;
    }
}

struct test_replay {
    piece_color winner;
    const char * log;
    const auto positions() const {
        return io::parse_game_positions(log);
    }
};

static test_replay replays[] = {
    {none, "e6 d6 c5 f6 e3 c3 e7 f2 b2 b6 b4 f4 g5 d7 c6 c7 c8 f7 f3 g3 a7 a1 g2 d8 d2 a5 f8 h2 d3 b7 g1 e8 h4 f1 b8 h3 h1 d1 g4 a8 a6 g8 a4 g7 c1 e2 h8 h6 h7 b3 f5 h5 e1 a3 c2 c4 g6 b1 a2 b5"},
};

void test_replay(const test_replay &r)
{
    assert(replay(r.positions(), r.winner));
}

void test_replays()
{
    for (auto &r : replays)
        test_replay(r);
}

void test_benchmark_winrate()
{
    vector<strategy> all = {
        strat::random_strategy,
        strat::random_strategy_with_borders_first,
        strat::random_strategy_with_corners_and_borders_first,
        strat::max_pieces,
        strat::minmax2
        // &strat::minmax4,
        // &strat::minmax2corners,
        // &strat::minmax4corners
    };

    assert(better_than(strat::random_strategy_with_borders_first,
        strat::random_strategy));
    // assert(better_than(strat::random_strategy_with_corners_and_borders_first,
    //     strat::random_strategy_with_borders_first, 0.05)); // eps = 0.05
    assert(better_than(strat::max_pieces, strat::random_strategy_with_borders_first, 0.05));
    assert(better_than(strat::minmax2, strat::max_pieces, 0.05));
    assert(better_than(strat::minmax2corners, strat::minmax2));
    assert(better_than(strat::minmax4, strat::minmax2));
    assert(better_than(strat::minmax4corners, strat::minmax4));
}

int main()
{
    test_positions();
    test_bitpos_direction();
    test_initial_condition_and_first_placement();
    test_parse_game_positions();
    test_replays();
    test_benchmark_winrate();
}