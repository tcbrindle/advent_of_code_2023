
#include "../aoc.hpp"

namespace {

struct card_t {
    std::vector<int> winning;
    std::vector<int> have;
    int copies = 1;
};

auto read_nums = [](std::string_view line) -> std::vector<int>
{
    return flux::split_string(line, ' ')
            .filter([](std::string_view s) { return !s.empty(); })
            .map([](std::string_view s) {
                return aoc::try_parse<int>(s).value();
            })
            .to<std::vector>();
};

auto parse_input = [](std::string_view input)
{
    return flux::split_string(input, '\n')
                .filter([](std::string_view line) { return !line.empty(); })
                .map([](std::string_view line) -> card_t {
                    auto colon = line.find(':');
                    auto bar = line.find('|', colon + 1);

                    return card_t{
                        .winning = read_nums(line.substr(colon + 1, bar - colon - 1)),
                        .have = read_nums(line.substr(bar+1))
                    };
                })
                .to<std::vector>();
};

auto matching_numbers = [](card_t const& card) -> auto {
    return flux::count_if(card.have, [&card](int x) {
        return flux::contains(card.winning, x);
    });
};

auto part1 = [](std::vector<card_t> const& cards) -> int {
    return flux::ref(cards)
            .map([](card_t const& card) -> int {
                auto count = matching_numbers(card);
                return count > 0 ? int(1u << (count - 1)) : 0;
            })
            .sum();
};

auto part2 = [](std::vector<card_t> cards) -> int {
    auto total = 0;

    for (auto card_idx : flux::ref(cards).cursors()) {
        auto const& card = cards.at(card_idx);

        for (auto i : flux::ints(1, 1 + matching_numbers(card))) {
            cards.at(card_idx + i).copies += card.copies;
        }

        total += card.copies;
    }

    return total;
};

constexpr auto& test_data =
R"(Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11)";

static_assert([] {
    auto test_cards = parse_input(test_data);
    return part1(test_cards) == 13 &&
           part2(test_cards) == 30;
}());

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const cards = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(cards));
    fmt::println("Part 2: {}", part2(cards));
}