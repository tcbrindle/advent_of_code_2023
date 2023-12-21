
#include "../aoc.hpp"

#include <set>

namespace {

using i64 = std::int64_t;

struct vec2 {
    i64 x, y;

    friend constexpr auto operator+(vec2 a, vec2 b) -> vec2 {
        return {a.x + b.x, a.y + b.y};
    }

    friend auto operator==(vec2, vec2) -> bool = default;
    friend auto operator<=>(vec2, vec2) = default;
};

constexpr vec2 north{0, -1};
constexpr vec2 east{1, 0};
constexpr vec2 south{0, 1};
constexpr vec2 west{-1, 0};

struct grid2d {
    std::string data;
    i64 size; // assumed square

    constexpr auto operator[](vec2 pos) const -> char
    {
        if (pos.x >= 0 && pos.x < size && pos.y >= 0 && pos.y < size) {
            return data.at(pos.y * size + pos.x);
        } else {
            return '#';
        }
    }

    constexpr auto tiled_at(vec2 pos) const -> char
    {
        pos.x %= size;
        pos.y %= size;

        if (pos.x < 0) {
            pos.x += size;
        }
        if (pos.y < 0) {
            pos.y += size;
        }

        return data.at(pos.y * size + pos.x);
    }
};

auto parse_input = [](std::string_view input) -> grid2d
{
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .size = flux::find(input, '\n')
    };
};

template <bool Tiled>
auto walk_garden = [](grid2d const& grid, i64 dist) -> i64
{
    auto start_idx = flux::find(grid.data, 'S');
    auto start_pos = vec2{start_idx % grid.size, start_idx / grid.size};

    std::set<vec2> cur{start_pos};

    while (dist-- > 0) {
        std::set<vec2> next;

        for (vec2 pos : cur) {
            for (vec2 off : {north, east, south, west}) {
                if constexpr (Tiled) {
                    if (grid.tiled_at(pos + off) != '#') {
                        next.insert(pos + off);
                    }
                } else {
                    if (grid[pos + off] != '#') {
                        next.insert(pos + off);
                    }
                }
            }
        }

        cur = std::move(next);
    }

    return cur.size();
};

auto part1 = [](grid2d const& grid, i64 dist) -> i64
{
    return walk_garden<false>(grid, dist);
};

// This is stolen wholesale from
// https://github.com/apprenticewiz/adventofcode/blob/main/2023/rust/day21b/src/main.rs
// (including the comment below)
auto part2 = [](grid2d const& grid) -> i64
{
    auto b0 = walk_garden<true>(grid, 65);
    auto b1 = walk_garden<true>(grid, 196);
    auto b2 = walk_garden<true>(grid, 327);

    i64 n = 202300;
    // the following formula comes from inv(A) * B = X,
    // where A is Vandemonde matrix:
    // [ 0 0 1 ]
    // [ 1 1 1 ]
    // [ 4 2 1 ]
    // and B is a column vector from the above values b0, b1, b2
    // credit to: https://gist.github.com/dllu/0ca7bfbd10a199f69bcec92f067ec94c
    // below uses Cramer's Rule to solve for x0, x1, x2
    double det_a = -2.0;
    double det_a0 = double(-b0) + 2.0 * double(b1) - double(b2);
    double det_a1 = 3.0 * double(b0) - 4.0 * double(b1) + double(b2);
    double det_a2 = -2.0 * double(b0);
    i64 x0 = i64(det_a0 / det_a);
    i64 x1 = i64(det_a1 / det_a);
    i64 x2 = i64(det_a2 / det_a);
    return x0 * n * n + x1 * n + x2;
};

constexpr auto& test_data =
R"(...........
.....###.#.
.###.##..#.
..#.#...#..
....#.#....
.##..S####.
.##..#...#.
.......##..
.##.#.####.
.##..##.##.
...........)";

}

int main(int argc, char** argv)
{
    {
        grid2d const test_grid = parse_input(test_data);
        assert(part1(parse_input(test_data), 6) == 16);

        assert(walk_garden<true>(test_grid, 6) == 16);
        assert(walk_garden<true>(test_grid, 10) == 50);
        assert(walk_garden<true>(test_grid, 50) == 1594);
        assert(walk_garden<true>(test_grid, 100) == 6536);
//        assert(walk_garden<true>(test_grid, 500) == 167004);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    grid2d const grid = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(grid, 64));
    fmt::println("Part 2: {}", part2(grid));
}