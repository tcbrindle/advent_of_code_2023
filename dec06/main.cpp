
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

auto calculate = [](i64 time, i64 dist) -> i64
{
    double disc = std::sqrt(time * time - 4 * dist);
    i64 min = std::floor((time - disc)/2 + 1.0);
    i64 max = std::ceil((time + disc)/2 - 1.0);
    return max - min + 1;
};

auto part1 = [](std::string_view input) -> i64
{
    auto read_numbers = [](std::string_view line) {
        return flux::split_string(line.substr(line.find(':') + 1), ' ')
            .filter([](auto s) { return !s.empty(); })
            .map(aoc::parse<int>);
    };

    auto nl = input.find('\n');
    auto times = input.substr(0, nl);
    auto distances = input.substr(nl + 1);

    return flux::zip(read_numbers(times), read_numbers(distances))
            .map(flux::unpack(calculate))
            .product();
};

auto part2 = [](std::string_view input) -> i64
{
    auto read_numbers = [](std::string_view line) -> i64 {
        return aoc::parse<i64>(flux::filter(line, is_digit));
    };

    auto nl = input.find('\n');

    return calculate(read_numbers(input.substr(0, nl)),
                     read_numbers(input.substr(nl + 1)));
};

constexpr auto& test_data =
R"(Time:      7  15   30
Distance:  9  40  200)";

}

int main(int argc, char** argv)
{
    // With libstdc++, we can test at compile time
#ifdef __GLIBCXX__
    static_assert(part1(test_data) == 288);
    static_assert(part2(test_data) == 71503);
#else
    assert(part1(test_data) == 288);
    assert(part2(test_data) == 71503);
#endif

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);
    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}
