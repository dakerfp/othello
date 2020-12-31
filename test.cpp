#include <cassert>

#include "othello.h"

using namespace std;

int main()
{
    othello::game g(4);

    assert(g.get_size() == 4);
    assert(g.player() == othello::white);

    assert(g.count_pieces(othello::white) == 2);
    assert(g.count_pieces(othello::black) == 2);
    assert(!g.is_game_over());
}