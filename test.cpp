
#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"
#include "benchmark.h"
#include "ai.h"
#include "io.h"

using namespace std;

void test_indexes()
{
    othello::bitmap8x8 bmp = othello::util::bit(42);
    othello::indexes idxs = {bmp};
    assert(bmp == othello::uint64(1) << 42);
    assert(idxs.size() == 1);

    bmp = 0;
    idxs = {bmp};
    assert(idxs.size() == 0);
    assert(idxs.begin() == idxs.end());

    bmp = othello::util::bit(7) | othello::util::bit(42);
    idxs = {bmp};
    assert(idxs.size() == 2);

    auto it = idxs.begin();
    assert(*it == othello::util::bit(7));
    ++it;
    assert(it.index() == 42);
    assert(*it == othello::util::bit(42));
    ++it;
    assert(*it == 0);
    assert(it == idxs.end());

    int count = 0;
    for (__attribute__((unused)) othello::bitpos p : idxs) {
        count++;
        if (count > 10)
            assert(0);
    }
    assert(count == 2);
}

void test_initial_condition_and_first_placement()
{
    othello::game g;

    assert(g.size == 8);
    assert(g.player() == othello::white);

    assert(g.count_pieces(othello::white) == 2);
    assert(g.count_pieces(othello::black) == 2);
    assert(g.count_pieces(othello::none) == 8 * 8 - 4);
    assert(!g.is_game_over());

    assert(g.can_play({5, 3}, othello::white));
    assert(!g.can_play({5, 3}, othello::black));
    assert(!g.can_play({3, 3}, othello::white));

    g.place_piece({5, 3});

    assert(!g.can_play({5, 3}, othello::black));
    assert(!g.can_play({5, 3}, othello::white));
    assert(g.player() == othello::black);
    assert(g.count_pieces(othello::white) == 4);
    assert(g.count_pieces(othello::black) == 1);
    assert(g.count_pieces(othello::none) == 8 * 8 - 5);
}

struct replay {
    othello::piece_color winner;
    const char * log;
    const vector<othello::pos> positions() const {
        return othello::io::parse_game_positions(log);
    }
};

void test_parse_game_positions()
{
    assert(othello::io::to_string({0,0}) == "a1");
    assert(othello::io::to_string({7,7}) == "h8");

    vector<othello::pos> positions = othello::io::parse_game_positions("a1 b2 c3 d4 e5 f6 g7 h8");
    assert(positions.size() == 8);
    for (unsigned i = 0; i < positions.size(); i++)
        assert(positions[i] == othello::pos(i, i));
}

static replay replays[] = {
    {othello::white, "d6 c6 f4 d3 d2 e6 c7 e3 f3 c5 b6 b7 a7 a6 f6 f2 b4 c8 a8 g7 g1 e2 d7 c3 f5 g2 h1 d8 a5 g5 c4 g3 h5 e7 h8 h6 b8 c1 f7 g4 b5 h2 e1 f1 e8 c2 h3 a4 a3 b2 h4 f8 a1 d1 b3 g6 g8 a2 b1 h7"},
};

void test_replay(const replay &r)
{
    assert(othello::replay(r.positions(), r.winner));
}

void test_replays()
{
    for (auto &r : replays)
        test_replay(r);
}

void test_benchmark_winrate()
{
    othello::random_strategy random;
    othello::random_strategy_with_borders_first random_with_borders_first;
    othello::random_strategy_with_corners_and_borders_first random_with_borders_and_corners_first;
    othello::maximize_score_strategy max_pieces;
    othello::minmax_strategy minmax2(othello::none, 2);
    othello::minmax_strategy minmax4(othello::none, 4);
    othello::minmax_strategy minmax2corners(othello::none, 2, othello::pieces_diff_score_with_borders_and_corners);
    othello::minmax_strategy minmax4corners(othello::none, 4, othello::pieces_diff_score_with_borders_and_corners);

    vector<othello::strategy *> all = {
        &random,
        &random_with_borders_first,
        &random_with_borders_and_corners_first,
        &max_pieces,
        &minmax2,
        &minmax4,
        &minmax2corners,
        &minmax4corners
    };

    assert(othello::better_than(&random_with_borders_first, &random));
    assert(othello::better_than(&random_with_borders_and_corners_first, &random_with_borders_first, 0.05)); // eps = 0.05
    assert(othello::better_than(&max_pieces, &random_with_borders_first, 0.05)); // eps = 0.05
    assert(othello::better_than(&max_pieces, &minmax2));
    assert(othello::better_than(&minmax2corners, &minmax2));
    // assert(othello::better_than(&minmax4, &minmax2));
    // assert(othello::better_than(&minmax4corners, &minmax4));
}

int main()
{
    test_indexes();
    test_initial_condition_and_first_placement();
    test_parse_game_positions();
    test_replays();
    test_benchmark_winrate();
}