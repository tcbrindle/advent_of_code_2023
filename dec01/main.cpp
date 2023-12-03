
#include "../aoc.hpp"

namespace {

/*
 * Part 1
 */

auto find_digits_part1 = [](std::string_view const line) -> int {
    constexpr std::string_view digits = "0123456789";

    auto first_digit_idx = line.find_first_of(digits);
    int first_digit = line.at(first_digit_idx) - '0';

    auto last_digit_idx = line.find_last_of(digits);
    int last_digit = line.at(last_digit_idx) - '0';

    return 10 * first_digit + last_digit;
};

auto part1 = [](std::string_view const input) -> int {
    return flux::split_string(input, '\n')
        .filter([](std::string_view const line) { return !line.empty(); })
        .map(find_digits_part1)
        .sum();
};

/*
 * Part 2
 */

constexpr auto digits_map = std::array<std::pair<std::string_view, int>, 18>{{
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {"5", 5},
    {"6", 6},
    {"7", 7},
    {"8", 8},
    {"9", 9},
    {"one", 1},
    {"two", 2},
    {"three", 3},
    {"four", 4},
    {"five", 5},
    {"six", 6},
    {"seven", 7},
    {"eight", 8},
    {"nine", 9}
}};

auto find_first_digit = [](std::string_view line) -> int {
    while (!line.empty()) {
        for (auto const& [str, value] : digits_map) {
            if (line.starts_with(str)) {
                return value;
            }
        }
        line.remove_prefix(1);
    }
    return 0;
};

auto find_last_digit = [](std::string_view line) -> int {
    while (!line.empty()) {
        for (auto const& [str, value] : digits_map) {
            if (line.ends_with(str)) {
                return value;
            }
        }
        line.remove_suffix(1);
    }
    return 0;
};

auto part2 = [](std::string_view const input) -> int {
    return flux::split_string(input, '\n')
        .map([](std::string_view const line) -> int {
            return 10 * find_first_digit(line) + find_last_digit(line);
        })
        .sum();
};

/*
 * Tests
 */

constexpr auto& test_data_p1 =
R"(1abc2
pqr3stu8vwx
a1b2c3d4e5f
treb7uchet)";

static_assert(part1(test_data_p1) == 142);

constexpr auto& test_data_p2 =
R"(two1nine
eightwothree
abcone2threexyz
xtwone3four
4nineeightseven2
zoneight234
7pqrstsixteen)";

static_assert(part2(test_data_p2) == 281);

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
