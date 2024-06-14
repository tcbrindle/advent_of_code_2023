
#include "../aoc.hpp"

#include <ankerl/unordered_dense.h>

namespace {

using i64 = std::int64_t;

struct grid2d {
    std::string data;
    i64 size; // assumed square

    constexpr auto operator[](i64 idx) -> char& { return data.at(idx); }
    constexpr auto operator[](i64 idx) const -> char { return data.at(idx); }

    // Returns a sequence-of-sequences of the row indices
    constexpr auto rows() const -> flux::sequence auto
    {
        return flux::ints(0, size * size).chunk(size);
    }

    // Returns a sequence-of-sequences of the column indices
    constexpr auto columns() const -> flux::sequence auto
    {
        return flux::ints(0, size * size)
                .map([sz = size](i64 i) { return (i % sz) * sz + (i/sz); })
                .chunk(size);
    }
};

auto parse_input = [](std::string_view input) -> grid2d
{
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .size = flux::find(input, '\n')
    };
};

// Roll a single row/column, mutating the grid in-place
auto do_roll = [](grid2d& grid, flux::sequence auto indices)
{
    auto col = flux::map(indices, [&grid](i64 i) -> char& { return grid[i]; });

    for (auto pos = col.first(); !col.is_last(pos); col.inc(pos)) {
        if (col[pos] == '.') {
            // fast-forward until we find the next non-empty position
            auto nonempty = flux::slice(col, col.next(pos), col.last())
                                .find_if_not(flux::pred::eq('.'));
            if (col.is_last(nonempty)) {
                break;
            } else if (col[nonempty] == 'O') {
                flux::swap_at(col, pos, nonempty);
            } else if (col[nonempty] == '#') {
                pos = nonempty;
            }
        }
    }
};

auto calculate_score = [](grid2d const& grid) -> i64
{
    return grid.columns()
            .map([&grid](auto indices) {
                return flux::zip(indices, flux::ints(0, grid.size + 1).reverse())
                    .filter([&grid](auto const& p) { return grid[p.first] == 'O'; })
                    .map([](auto const& p) -> i64 { return p.second; })
                    .sum();
            })
            .sum();
};

auto part1 = [](grid2d grid) -> i64
{
    // Roll the grid north once
    grid.columns().for_each([&grid](auto col) { do_roll(grid, col); });
    return calculate_score(grid);
};

// Rolls the grid in four directions, mutating it in-place
auto roll_grid = [](grid2d& grid)
{
    // Roll north
    grid.columns().for_each([&grid](auto col) { do_roll(grid, col); });
    // Roll west
    grid.rows().for_each([&grid](auto row) { do_roll(grid, row); });
    // Roll south
    grid.columns().for_each([&grid](auto col) { do_roll(grid, flux::reverse(col)); });
    // Roll east
    grid.rows().for_each([&grid](auto row) { do_roll(grid, flux::reverse(row)); });
};

auto part2 = [](grid2d grid) -> i64
{
    ankerl::unordered_dense::map<std::string, i64> states;
    states[grid.data] = 0;

    i64 loop_entry = -1;
    i64 loop_len = -1;

    for (i64 i : flux::ints(1)) {
        roll_grid(grid);

        auto [iter, inserted] = states.try_emplace(grid.data, i);
        if (!inserted) {
            //fmt::println("Found repeated state after {} cycles (first was {})",
            //             i, iter->second);
            loop_entry = iter->second;
            loop_len = i - loop_entry;
            break;
        }
    }

    // Perform a few more iterations to get to the right point in the cycle
    i64 remaining = (1'000'000'000 - loop_entry) % loop_len;
    for (i64 _ : flux::ints(0, remaining)) {
        roll_grid(grid);
    }

    return calculate_score(grid);
};

constexpr auto& test_data =
R"(O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....)";

}

int main(int argc, char** argv)
{
    {
        auto test_input = parse_input(test_data);
        assert(part1(test_input) == 136);
        assert(part2(test_input) == 64);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto input = parse_input(aoc::string_from_file(argv[1]));
    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}