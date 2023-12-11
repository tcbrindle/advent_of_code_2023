
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

struct position { i64 x, y; };

auto distance = [](position from, position to) {
    auto abs = [](auto n) { return n < 0 ? -n : n; };
    return abs(from.x - to.x) + abs(from.y - to.y);
};

template <i64 Expansion>
auto parse_input = [](std::string_view input) -> std::vector<position>
{
    std::vector<position> galaxies;
    auto n_cols = flux::find(input, '\n');
    auto cols_with_galaxy = std::vector<bool>(n_cols, false);
    i64 y = 0;

    for (auto line : flux::split_string(input, '\n')) {
        i64 x = 0;
        bool row_has_galaxy = false;
        for (char c : line) {
            if (c == '#') {
                galaxies.push_back({x, y});
                row_has_galaxy = true;
                cols_with_galaxy[x] = true;
            }
            ++x;
        }

        ++y;
        if (!row_has_galaxy) {
            y += Expansion - 1;
        }
    }

    // Now go through and "expand" the x-coords where there are no galaxies
    for (auto i : flux::ints(0, n_cols).reverse()) {
        if (!cols_with_galaxy[i]) {
            for (auto& [x, _] : galaxies) {
                if (x > i) {
                    x += Expansion - 1;
                }
            }
        }
    }

    return galaxies;
};

template <i64 Expansion>
auto calculate_distances = [](std::string_view input) -> i64
{
    auto const galaxies = parse_input<Expansion>(input);

    i64 sum = 0;
    for (auto i : flux::ints(0, flux::size(galaxies))) {
        for (auto j : flux::ints(i + 1, flux::size(galaxies))) {
            sum += distance(galaxies.at(i), galaxies.at(j));
        }
    }
    return sum;
};

auto part1 = calculate_distances<2>;
auto part2 = calculate_distances<1'000'000>;

constexpr auto& test_data =
R"(...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....)";

static_assert(calculate_distances<2>(test_data) == 374);
static_assert(calculate_distances<10>(test_data) == 1030);
static_assert(calculate_distances<100>(test_data) == 8410);

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}