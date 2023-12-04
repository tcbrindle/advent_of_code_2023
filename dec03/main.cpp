
#include "../aoc.hpp"

#include <charconv>

struct grid_t {
    struct position {
        int x;
        int y;

        friend auto operator==(position const&, position const&) -> bool = default;
        friend auto operator<=>(position const&, position const&) = default;
    };

    struct number {
        int start_x;
        int end_x; // inclusive, not past-the-end!
        int y;
        int value;
    };

    std::map<position, char> symbols;
    std::vector<number> numbers;
};

auto parse_input = [](std::string_view input) -> grid_t
{
    int x = 0;
    int y = 0;
    grid_t grid{};

    while(!input.empty()) {
        char c = input.front();
        if (c == '\n') {
            ++y;
            x = 0;
            input.remove_prefix(1);
        } else if (c == '.') {
            ++x;
            input.remove_prefix(1);
        } else if (::isdigit(c)) {
            // fast-forward till we find a non-digit
            auto end_idx = flux::find_if_not(input, ::isdigit);

            int val = *aoc::try_parse<int>(input.substr(0, end_idx));

            grid.numbers.emplace_back(x, x + end_idx - 1, y, val);

            x += int(end_idx);
            input.remove_prefix(end_idx);
        } else {
            grid.symbols[{x, y}] = c;
            ++x;
            input.remove_prefix(1);
        }
    }

    return grid;
};

// for each number
//   - check each position around the boundary to see if the symbols map has an
//     entry, and ignore those which don't
//   - map each number to its value
//   - sum the values
auto part1 = [](grid_t const& grid) -> int
{
    return flux::ref(grid.numbers)
            .filter([&grid](grid_t::number const& num) -> bool {
                return flux::cartesian_product(flux::ints(num.start_x-1, num.end_x+2),
                                               flux::ints(num.y-1, num.y+2))
                        .any(flux::unpack([&](int x, int y) {
                            return grid.symbols.contains({x, y});
                        }));
            })
            .map(&grid_t::number::value)
            .sum();
};

// for each symbol in grid.symbols
//  - filter, ignoring anything that is not '*'
//  - look through the numbers vector and find all the entries bordering the '*'
//  - if there are not exactly two of them, move on
//  - otherwise, take the product of the two values
//  - finally, sum up all the products
auto part2 = [](grid_t const& grid)
{
    return flux::from_range(grid.symbols)
                .filter([](auto const& pair) { return pair.second == '*'; })
                .map([&grid](auto const& pair) -> std::vector<int> {
                    auto [x, y] = pair.first;
                    return flux::ref(grid.numbers)
                            .filter([x, y](grid_t::number const& num) {
                                return x >= num.start_x - 1 &&
                                       x <= num.end_x + 1 &&
                                       y >= num.y - 1 &&
                                       y <= num.y + 1;
                            })
                            .map(&grid_t::number::value)
                            .template to<std::vector>();
                 })
                .filter([](auto const& seq) { return seq.size() == 2; })
                .map(flux::product)
                .sum();
};

constexpr auto& test_data =
R"(467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)";

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    {
        auto const test_grid = parse_input(test_data);
        assert(part1(test_grid) == 4361);
        assert(part2(test_grid) == 467835);
    }

    auto grid = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(grid));
    fmt::println("Part 2: {}", part2(grid));
}

