
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

template <i64 RequiredDiffs>
auto find_reflection_idx = [](flux::sequence auto seq) -> i64
{
    auto idx = flux::ints(1, flux::size(seq)).find_if([&seq](i64 idx) {
        auto top = flux::slice(seq, 0, idx).reverse().flatten();
        auto bottom = flux::slice(seq, idx, flux::last).flatten();

        // Use for_each_while so we can bail out if we find too many diffs
        i64 diffs = 0;
        flux::zip_for_each_while([&diffs](char a, char b) {
            diffs += (a != b);
            return diffs <= RequiredDiffs;
        }, std::move(top), std::move(bottom));

        return diffs == RequiredDiffs;
    });

    return idx == flux::size(seq) ? 0 : idx;
};

template <i64 RequiredDiffs>
auto find_reflection = [](grid_t const& grid) -> i64 {

    auto horiz_idx = find_reflection_idx<RequiredDiffs>(flux::ref(grid));
    if (horiz_idx != 0) {
        return 100 * horiz_idx;
    }

    auto columns = flux::ints(0, flux::size(grid.at(0)))
                    .map([&grid](auto y) {
                           return flux::ref(grid)
                                     .map([y](auto const& str) { return str.at(y); });
                    });

    return find_reflection_idx<RequiredDiffs>(std::move(columns));
};

auto part1 = [](std::vector<grid_t> const& input) -> i64
{
    return flux::ref(input).map(find_reflection<0>).sum();
};

auto part2 = [](std::vector<grid_t> const& input) -> i64
{
    return flux::ref(input).map(find_reflection<1>).sum();
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