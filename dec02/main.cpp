
#include "../aoc.hpp"

#include "../extern/ctre.hpp"

namespace {

using namespace std::string_view_literals;

struct game {
    int id;
    int red;
    int green;
    int blue;
};

auto parse_draw = [](std::string_view draw) -> game
{
    game g{};
    for (auto [match, n, colour] : ctre::range<R"(\s*(\d+) (\w+),?)">(draw)) {
        if (colour == "red"sv) {
            g.red = n.to_number();
        } else if (colour == "green"sv) {
            g.green = n.to_number();
        } else if (colour == "blue"sv) {
            g.blue = n.to_number();
        }
    }
    return g;
};

auto merge_draws = [](game lhs, game rhs) -> game {
    return game {
        .red = std::max(lhs.red, rhs.red),
        .green = std::max(lhs.green, rhs.green),
        .blue = std::max(lhs.blue, rhs.blue)
    };
};

auto parse_line = [](std::string_view line) -> game
{
    auto [match, id, draws] = ctre::match<R"(^Game (\d+): (.*)$)">(line);

    game g = flux::split_string(draws, ';')
                 .map(parse_draw)
                 .fold(merge_draws, game{});

    g.id = id.to_number();
    return g;
};

auto parse_input = [](std::string_view input) -> std::vector<game>
{
    return flux::split_string(input, '\n')
                .map(parse_line)
                .to<std::vector>();
};

auto part1 = [](flux::sequence auto const& games) -> int {
    auto possible = [](game const& g) {
        return g.red <= 12 && g.green <= 13 && g.blue <= 14;
    };

    return flux::ref(games)
            .filter(possible)
            .map(&game::id)
            .sum();
};

auto part2 = [](flux::sequence auto const& games) -> int {
    auto power = [](game const& g) { return g.red * g.green * g.blue; };
    return flux::ref(games).map(power).sum();
};

constexpr auto& test_data =
R"(Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green)";

auto test = [] -> bool {
    auto games = parse_input(test_data);
    auto p1 = part1(games);
    auto p2 = part2(games);

    return p1 == 8 && p2 == 2286;
};
static_assert(test());

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const games = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(games));
    fmt::println("Part 2: {}", part2(games));
}