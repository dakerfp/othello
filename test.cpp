#include <cassert>

#include "othello.h"

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

int main()
{
    test_initial_condition_and_first_placement();
}