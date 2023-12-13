
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

using grid_t = std::vector<std::string>;

auto parse_input = [](std::string_view input) -> std::vector<grid_t>
{
    return flux::split_string(input, "\n\n")
            .map([](std::string_view str) -> grid_t {
                return flux::split_string(str, '\n')
                         .filter([](auto sv) { return !sv.empty(); })
                         .to<grid_t>();
            })
            .to<std::vector>();
};

auto reflection_idx = [](flux::bidirectional_sequence auto seq, i64 ignore) -> i64
{
    for (auto idx : flux::ints(1, flux::size(seq))) {
        if (idx == ignore) {
            continue;
        }
        flux::sequence auto top = flux::slice(seq, 0, idx).reverse();
        flux::sequence auto bottom = flux::slice(seq, idx, flux::last);

        auto [a, b] = flux::zip_find_if(std::not_fn(flux::equal), top, bottom);

        if (top.is_last(a) || bottom.is_last(b)) {
            return idx;
        }
    }
    return 0;
};

auto find_reflection = [](grid_t const& grid, i64 ignore = 0) -> i64 {

    auto horiz = reflection_idx(flux::ref(grid), ignore/100);
    if (horiz != 0) {
        return 100 * horiz;
    }

    auto columns = flux::ints(0, flux::size(grid.at(0)))
                    .map([&grid](auto y) {
                           return flux::ref(grid)
                                     .map([y](auto const& str) { return str.at(y); });
                    });

    return reflection_idx(flux::ref(columns), ignore);
};

auto part1 = [](std::vector<grid_t> const& input) -> i64
{
    return flux::ref(input).map(find_reflection).sum();
};

auto find_smudge = [](grid_t grid) -> i64
{
    auto flip = [](char& c) { c == '.' ? c = '#' : c = '.'; };

    // Let's just try brute force
    auto height = flux::size(grid);
    auto width = flux::size(grid.at(0));
    auto old_r = find_reflection(grid);

    for (auto [x, y] : flux::cartesian_product(flux::ints(0, width),
                                               flux::ints(0, height))) {
        flip(grid.at(y).at(x));
        auto r = find_reflection(grid, old_r);
        if (r > 0) {
            return r;
        }
        flip(grid.at(y).at(x));
    }

    throw std::runtime_error("Could not find smudge location!");
};

auto part2 = [](std::vector<grid_t> const& input) -> i64
{
    return flux::ref(input).map(find_smudge).sum();
};

constexpr auto& test_data =
R"(#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#)";

static_assert([] {
    auto const test_input = parse_input(test_data);
    return part1(test_input) == 405
        && part2(test_input) == 400;
}());

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}