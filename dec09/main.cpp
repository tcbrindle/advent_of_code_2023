
#include "../aoc.hpp"

namespace {

auto difference = [](int a, int b) { return b - a; };

auto parse_input = [](std::string_view input) -> std::vector<std::vector<int>>
{
    return flux::split_string(input, '\n')
              .filter([](auto line) { return !line.empty(); })
              .map([](std::string_view line) -> std::vector<int> {
                  return flux::split_string(line, ' ').map(aoc::parse<int>).to<std::vector>();
              })
              .to<std::vector>();
};

constexpr auto recurse_down(std::vector<int> const& seq) -> int
{
    auto differences = flux::pairwise_map(flux::ref(seq), difference).to<std::vector>();

    if (flux::all(differences, flux::pred::eq(0))) {
        return 0;
    } else {
        return recurse_down(differences) + differences.back();
    }
}

auto part1 = [](std::vector<std::vector<int>> const& input) -> int
{
    return flux::ref(input)
            .map([](std::vector<int> const& nums) {
                return recurse_down(nums) + nums.back();
            })
            .sum();
};

auto part2 = [](std::vector<std::vector<int>> input) -> int
{
    flux::mut_ref(input).for_each(flux::inplace_reverse);
    return part1(input);
};

constexpr auto& test_data =
R"(0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45)";

static_assert(part1(parse_input(test_data)) == 114);
static_assert(part2(parse_input(test_data)) == 2);

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