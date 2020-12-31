#include <cassert>
#include <memory>
#include <iostream>

#include "othello.h"
#include "ai.h"

using namespace std;

void test_initial_condition_and_first_placement()
{
    othello::game g(4);

    assert(g.get_size() == 4);
    assert(g.player() == othello::white);

    assert(g.count_pieces(othello::white) == 2);
    assert(g.count_pieces(othello::black) == 2);
    assert(!g.is_game_over());

    assert(g.can_play({3, 1}));
    assert(!g.can_play({3, 3}));

    g.place_piece({3, 1});

    assert(!g.can_play({3, 1}));
    assert(g.player() == othello::black);
    assert(g.count_pieces(othello::white) == 4);
    assert(g.count_pieces(othello::black) == 1);
}

template<typename A, typename B>
double winrate(int n=1000, int size=8)
{
    othello::game game(size);
    double win_score = 0;
    unique_ptr<othello::strategy> a = make_unique<A>();
    unique_ptr<othello::strategy> b = make_unique<B>();

    for (int i = 0; i < n; i++) {
        bool swap = i % 2 != 0;
        std::unique_ptr<othello::strategy> const &strategy_white = swap ? b : a;
        std::unique_ptr<othello::strategy> const &strategy_black = swap ? a : b;
        game.init();
        strategy_white->reset(othello::white);
        strategy_black->reset(othello::black);
        switch (play(game, strategy_white, strategy_black)) {
        case othello::white:
            win_score += swap ? 0 : 1;
            break;
        case othello::black:
            win_score += swap ? 1 : 0;
            break;
        default:
            win_score += 0.5;
        }
    }
    return win_score / n;
}

template<typename A, typename B>
void print_winrate_score(int n=10000, int size=8)
{
    cout << A::description
        << " vs "
        << B::description
        << ": "
        << winrate<A, B>()
        << endl;
}

void benchmark_winrate()
{
    print_winrate_score<
        othello::random_strategy,
        othello::random_strategy>();
    print_winrate_score<
        othello::random_strategy,
        othello::random_strategy_with_borders_first>();
    print_winrate_score<
        othello::random_strategy,
        othello::random_strategy_with_corners_and_borders_first>();
    print_winrate_score<
        othello::random_strategy_with_borders_first,
        othello::random_strategy_with_corners_and_borders_first>();
}

int main()
{
    test_initial_condition_and_first_placement();
    benchmark_winrate();
}