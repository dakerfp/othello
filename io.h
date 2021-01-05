#ifndef OTHELLO_IO_H
#define OTHELLO_IO_H

#include "othello.h"

#include <string>
#include <vector>
#include <sstream>

namespace othello::io {

constexpr int clamp(int n)
{
    if (n < 0)
        return 0;
    if (n >= 8)
        return 7;
    return n;
}

constexpr char alpha_from_index(int index)
{
    return 'a' + clamp(index);
}

constexpr int index_from_alpha(char alpha)
{
    return clamp(alpha - 'a');
}

constexpr int index_from_digit(char digit)
{
    return clamp(digit - '1');
}

std::string to_string(const othello::pos &p)
{
    return std::string(1, alpha_from_index(p.x)) + std::to_string(p.y + 1);
}

bool parse_pos(std::string s, othello::pos &pos)
{
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

    if (s.size() != 2)
        return false;

    if (std::isalpha(s[0]) && std::isdigit(s[1])) {
        pos.x = index_from_alpha(s[0]);
        pos.y = index_from_digit(s[1]);
        return true;
    }
    if (std::isalpha(s[1]) && std::isdigit(s[0])) {
        pos.x = index_from_alpha(s[1]);
        pos.y = index_from_digit(s[0]);
        return true;
    }
    return false;
}

std::vector<pos> parse_game_positions(std::string line)
{
    std::vector<pos> positions;
    std::stringstream lines(line);
    std::string token;
    while (lines >> token) {
        pos p;
        if (!parse_pos(token, p))
            return std::vector<pos>();
        positions.push_back(p);
    }
    return positions;
}

}

#endif // OTHELLO_IO_H