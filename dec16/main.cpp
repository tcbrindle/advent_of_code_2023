
#include "../aoc.hpp"

#include <set>
#include <stack>

namespace {

using i64 = std::int64_t;

enum class direction : std::uint8_t {
    north, east, south, west
};

struct position {
    i64 x, y;

    constexpr auto operator+=(direction d) -> position&
    {
        switch(d) {
        case direction::north: --y; break;
        case direction::east: ++x; break;
        case direction::south: ++y; break;
        case direction::west: --x; break;
        }
        return *this;
    }

    auto operator==(position const&) const -> bool = default;
    auto operator<=>(position const&) const = default;

    friend constexpr auto operator+(position p, direction d) -> position
    {
        return p += d;
    }

};

struct grid2d {
    std::string data;
    i64 size; // assumed square

    constexpr auto to_idx(position pos) const -> i64
    {
        return pos.y * size + pos.x;
    }

    constexpr auto operator[](position pos) const -> char
    {
        return data.at(to_idx(pos));
    }

    constexpr auto in_bounds(position pos) const -> bool
    {
        return pos.x >= 0 && pos.x < size && pos.y >= 0 && pos.y < size;
    }
};

auto parse_input = [](std::string_view input) -> grid2d
{
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .size = flux::find(input, '\n')
    };
};

auto fire_beam = [](grid2d const& grid, position start_pos, direction start_dir) -> i64
{
    std::set<std::pair<position, direction>> past_positions;
    std::vector<bool> energised(grid.data.size(), false);

    std::stack<std::pair<position, direction>> beams;
    beams.push({start_pos, start_dir});

    while (!beams.empty()) {
        auto [pos, dir] = beams.top();
        beams.pop();

        while (true) {
            if (!grid.in_bounds(pos)) {
                break;
            }

            if (auto [iter, inserted] = past_positions.insert({pos, dir}); !inserted) {
                break;
            }
            energised.at(grid.to_idx(pos)) = true;

            char c = grid[pos];

            switch (c) {
            case '.':
                break;

            case '\\':
                switch (dir) {
                case direction::north: dir = direction::west; break;
                case direction::east: dir = direction::south; break;
                case direction::south: dir = direction::east; break;
                case direction::west: dir = direction::north; break;
                }
                break;

            case '/':
                switch (dir) {
                case direction::north: dir = direction::east; break;
                case direction::east: dir = direction::north; break;
                case direction::south: dir = direction::west; break;
                case direction::west: dir = direction::south; break;
                }
                break;

            case '-':
                switch (dir) {
                case direction::east: [[fallthrough]];
                case direction::west: break;
                case direction::north: [[fallthrough]];
                case direction::south: {
                    beams.push({pos + direction::west, direction::west});
                    dir = direction::east;
                }
                }
                break;

            case '|':
                switch (dir) {
                case direction::north: [[fallthrough]];
                case direction::south: break;
                case direction::east: [[fallthrough]];
                case direction::west: {
                    beams.push({pos + direction::north, direction::north});
                    dir = direction::south;
                }
                }
                break;

            default: throw std::runtime_error("Unrecognised character in grid!");
            }

            pos += dir;
        }
    }

    return flux::from_crange(energised).count_eq(true);
};

constexpr auto part1 = [](grid2d const& grid) -> i64
{
    return fire_beam(grid, {0, 0}, direction::east);
};

constexpr auto part2 = [](grid2d const& grid) -> i64
{
    auto top = flux::ints(0, grid.size).map([](i64 i) {
        return std::pair(position{i, 0}, direction::south);
    });
    auto bottom = flux::ints(0, grid.size).map([sz = grid.size](i64 i) {
        return std::pair(position{i, sz}, direction::north);
    });
    auto left = flux::ints(0, grid.size).map([](i64 i) {
        return std::pair(position{0, i}, direction::east);
    });
    auto right = flux::ints(0, grid.size).map([sz = grid.size](i64 i) {
        return std::pair(position{sz, i}, direction::west);
    });

    return flux::chain(std::move(top), std::move(bottom), std::move(left), std::move(right))
              .map([&grid](auto pair) { return fire_beam(grid, pair.first, pair.second); })
              .max()
              .value();
};

constexpr auto& test_data =
R"(.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....)";

}

int main(int argc, char** argv)
{
    {
        auto const test_input = parse_input(test_data);
        assert(part1(test_input) == 46);
        assert(part2(test_input) == 51);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}