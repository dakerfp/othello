
#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"
#include "benchmark.h"
#include "ai.h"
#include "io.h"

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
    assert(g.player() == white);

    assert(g.count<white>() == 2);
    assert(g.count<black>() == 2);
    assert(g.count<none>() == 8 * 8 - 4);
    assert(!g.is_game_over());

    assert(g.can_play({5, 3}, white));
    assert(!g.can_play({5, 3}, black));
    assert(!g.can_play({3, 3}, white));

    g.place_piece({5, 3});

    assert(!g.can_play({5, 3}, black));
    assert(!g.can_play({5, 3}, white));
    assert(g.player() == black);
    assert(g.count<white>() == 4);
    assert(g.count<black>() == 1);
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
}

struct test_replay {
    piece_color winner;
    const char * log;
    const auto positions() const {
        return io::parse_game_positions(log);
    }
};

static test_replay replays[] = {
    {white, "d6 c6 f4 d3 d2 e6 c7 e3 f3 c5 b6 b7 a7 a6 f6 f2 b4 c8 a8 g7 g1 e2 d7 c3 f5 g2 h1 d8 a5 g5 c4 g3 h5 e7 h8 h6 b8 c1 f7 g4 b5 h2 e1 f1 e8 c2 h3 a4 a3 b2 h4 f8 a1 d1 b3 g6 g8 a2 b1 h7"},
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
    random_strategy random;
    random_strategy_with_borders_first random_with_borders_first;
    random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    maximize_score_strategy max_pieces;
    minmax_strategy minmax2(none, 2);
    minmax_strategy minmax4(none, 4);
    minmax_strategy minmax2corners(none, 2, pieces_diff_score_with_borders_and_corners);
    minmax_strategy minmax4corners(none, 4, pieces_diff_score_with_borders_and_corners);

    vector<strategy *> all = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &minmax2,
        &minmax4,
        &minmax2corners,
        &minmax4corners
    };

    assert(better_than(&random_with_borders_first, &random));
    assert(better_than(&random_with_borders_and_corners_first, &random_with_borders_first, 0.05)); // eps = 0.05
    assert(better_than(&max_pieces, &random_with_borders_first, 0.05)); // eps = 0.05
    // assert(better_than(&max_pieces, &minmax2));
    assert(better_than(&minmax2corners, &minmax2));
    assert(better_than(&minmax4, &minmax2));
    // assert(better_than(&minmax4corners, &minmax4));
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