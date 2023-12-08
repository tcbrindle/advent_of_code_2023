
#include "../aoc.hpp"

#include <numeric>

namespace {

using i64 = std::int64_t;

struct node {
    std::string left;
    std::string right;
};

using node_map = std::unordered_map<std::string, node>;

auto parse_input = [](std::string_view input) -> std::pair<std::string, node_map>
{
    auto nl = input.find("\n\n");

    auto instructions = std::string(input.substr(0, nl));

    auto map = flux::split_string(input.substr(nl + 2), '\n')
                .filter([](auto line) { return !line.empty(); })
                .map([](std::string_view line) {
                       return std::pair(std::string(line.substr(0, 3)),
                                        node{std::string(line.substr(7, 3)),
                                             std::string(line.substr(12, 3))});
                })
                .to<node_map>();

    return std::pair(std::move(instructions), std::move(map));
};

auto find_path_length = [](std::string where,
                           std::string_view instructions,
                           node_map const& nodes) -> i64
{
    i64 counter = 0;
    for (char inst : flux::cycle(instructions)) {
        if (where.back() == 'Z') {
            break;
        }
        ++counter;
        node const& n = nodes.at(where);
        where = (inst == 'L') ? n.left : n.right;
    }
    return counter;
};

auto part1 = [](std::string_view instructions, node_map const& nodes) -> i64
{
    return find_path_length("AAA", instructions, nodes);
};

auto part2 = [](std::string_view instructions, node_map const& nodes) -> i64
{
    return  flux::from_range(nodes)
                .map(&node_map::value_type::first)
                .filter([](std::string_view s) { return s.back() == 'A'; })
                .map([&](std::string const& from) {
                    return find_path_length(from, instructions, nodes);
                })
                .fold([](i64 a, i64 b) { return std::lcm(a, b); }, 1);
};

constexpr auto& test_data1 =
R"(RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ))";

constexpr auto& test_data2 =
R"(LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ))";

constexpr auto& test_data3 =
R"(LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX))";

}

int main(int argc, char** argv)
{
    // Alas, no constexpr tests today because of std::unordered_map
    {
        auto const [instr, map] = parse_input(test_data1);
        assert(part1(instr, map) == 2);
    }

    {
        auto const [instr, map] = parse_input(test_data2);
        assert(part1(instr, map) == 6);
    }

    {
        auto const [instr, map] = parse_input(test_data3);
        assert(part2(instr, map) == 6);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const [instructions, nodes] = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(instructions, nodes));
    fmt::println("Part 2: {}", part2(instructions, nodes));
}