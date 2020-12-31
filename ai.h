
#include "othello.h"

#include <cstdlib>

namespace othello {

class strategy
{
private:
    piece_color color;

protected:
    virtual pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) = 0;

public:
    strategy(piece_color color_)
        : color(color_)
    {}
    piece_color player() const { return color; }
    void set_piece_color(piece_color pc) { color = pc; }
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

class random_strategy : public strategy
{
public:
    random_strategy(piece_color color)
        : strategy(color)
    {}

    pos choose_piece_position(const game &g, const std::vector<pos> &possible_positions) override
    {
        int rand_pos = rand() % possible_positions.size();
        return possible_positions[rand_pos];
    }
};

}