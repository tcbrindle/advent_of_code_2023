
#include "../aoc.hpp"

namespace {

auto parse_input = [](std::string_view input) -> std::vector<std::vector<int>>
{
    return flux::split_string(input, '\n')
              .filter([](auto line) { return !line.empty(); })
              .map([](std::string_view line) -> std::vector<int> {
                  return flux::split_string(line, ' ').map(aoc::parse<int>).to<std::vector>();
              })
              .to<std::vector>();
};

auto part1 = [](std::vector<std::vector<int>> const& input) -> int
{
    return flux::ref(input)
            .map([](std::vector<int> vec) {
                    for (auto i : flux::ints(1, vec.size()).reverse()) {
                        for (auto j : flux::ints(0, i)) {
                            vec.at(j) = vec.at(j+1)  - vec.at(j);
                        }
                    }
                    return flux::sum(vec);
            })
            .sum();
};

auto part2 = [](std::vector<std::vector<int>> input) -> int
{
    flux::for_each(input, flux::inplace_reverse);
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