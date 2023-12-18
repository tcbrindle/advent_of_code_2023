
#include "../aoc.hpp"

#include <complex>

namespace {

using i64 = std::int64_t;

struct vec2 {
    i64 x, y;

    friend constexpr auto operator+(vec2 lhs, vec2 rhs) -> vec2
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }
};

auto parse_line_p1 = [](std::string_view line) -> std::pair<vec2, i64>
{
    char dir = line.at(0);
    int dist = aoc::parse<int>(line.substr(2, line.find('(')));
    switch (dir) {
    case 'U': return {{0, -dist}, dist};
    case 'D': return {{0, dist}, dist};
    case 'L': return {{-dist, 0}, dist};
    case 'R': return {{dist, 0}, dist};
    }
    throw std::runtime_error("Unknown direction");
};

auto parse_line_p2 = [](std::string_view line) -> std::pair<vec2, i64>
{
    auto num = line.substr(line.find('#') + 1, 5);

    i64 dist = 0;
    std::from_chars(num.data(), num.data() + 5, dist, 16);

    char dir = line.at(line.find(')') - 1);

    switch (dir) {
    case '3': return {{0, -dist}, dist}; break;
    case '1': return {{0, dist}, dist}; break;
    case '2': return {{-dist, 0}, dist}; break;
    case '0': return {{dist, 0}, dist}; break;
    }
    throw std::runtime_error("Unknown direction");
};

template <auto& ParseFn>
auto calculate_area = [](std::string_view input) -> i64
{
    using state = std::pair<vec2, i64>;

    auto area = flux::split_string(input, '\n')
        .filter(std::not_fn(flux::is_empty))
        .fold([](state s, std::string_view line) -> state {
             auto [p1, area] = s;
             auto [diff, len] = ParseFn(line);
             auto p2 = last_pos + diff;

             area += len + (p1.x * p2.y - p1.x * p2.y);

             return {p2, area};
         }, state{}).second;

    return 1 + area/2;
};

auto part1 = calculate_area<parse_line_p1>;
auto part2 = calculate_area<parse_line_p2>;

constexpr auto& test_data =
R"(R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3))";

static_assert(part1(test_data) == 62);
static_assert(part2(test_data) == 952408144115);

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input_str = aoc::string_from_file(argv[1]);

    fmt::println("Part 1: {}", part1(input_str));
    fmt::println("Part 2: {}", part2(input_str));
}