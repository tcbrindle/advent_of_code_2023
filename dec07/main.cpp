
#include "../aoc.hpp"

namespace {

using hand_t = std::string;

enum class rules : bool { no_jokers, with_jokers };

enum class hand_kind {
    high_card, one_pair, two_pair, three_of_a_kind, full_house,
    four_of_a_kind, five_of_a_kind
};

auto parse_input = [](std::string_view input) -> std::vector<std::pair<hand_t, int>>
{
    return flux::split_string(input, '\n')
            .filter([](auto line) { return !line.empty(); })
            .map([](std::string_view line) {
                return std::pair(std::string(line.substr(0, 5)), aoc::parse<int>(line.substr(6)));
            })
            .cache_last()
            .to<std::vector>();
};

auto evaluate_hand_no_jokers = [](hand_t const& hand) -> hand_kind {
    auto sum = flux::ref(hand)
                  .map([&hand](char c) { return flux::count_eq(hand, c); })
                  .sum();

    switch (sum) {
    case 25: return hand_kind::five_of_a_kind;
    case 17: return hand_kind::four_of_a_kind;
    case 13: return hand_kind::full_house;
    case 11: return hand_kind::three_of_a_kind;
    case 9: return hand_kind::two_pair;
    case 7: return hand_kind::one_pair;
    case 5: return hand_kind::high_card;
    default: throw std::runtime_error(fmt::format("Card '{}' has sum {}", hand, sum));
    }
};

auto evaluate_hand_with_jokers = [](hand_t const& hand) -> hand_kind {
    auto n_jokers = flux::count_eq(hand, 'J');

    auto sum = flux::ref(hand)
                   .filter(flux::pred::neq('J'))
                   .map([&hand](char c) { return flux::count_eq(hand, c); })
                   .sum();

    switch (n_jokers) {
    case 5: return hand_kind::five_of_a_kind;
    case 4: return hand_kind::five_of_a_kind;
    case 3:
        if (sum == 4) { return hand_kind::five_of_a_kind; }
        else if (sum == 2) { return hand_kind::four_of_a_kind; }
        break;
    case 2:
        if (sum == 9) { return hand_kind::five_of_a_kind; }
        else if (sum == 5) { return hand_kind::four_of_a_kind; }
        else if (sum == 3) { return hand_kind::three_of_a_kind; }
        break;
    case 1:
        if (sum == 16) { return hand_kind::five_of_a_kind; }
        else if (sum == 10) { return hand_kind::four_of_a_kind; }
        else if (sum == 8) { return hand_kind::full_house; }
        else if (sum == 6) { return hand_kind::three_of_a_kind; }
        else if (sum == 4) { return hand_kind::one_pair; }
        break;
    case 0: return evaluate_hand_no_jokers(hand);
    default: break;
    }

    throw std::runtime_error(fmt::format("Card '{}' has sum {}", hand, sum));
};

template <rules Rules>
constexpr auto evaluate_hand(hand_t const& hand) -> hand_kind {
    if constexpr (Rules == rules::no_jokers) {
        return evaluate_hand_no_jokers(hand);
    } else {
        return evaluate_hand_with_jokers(hand);
    }
};

template <rules Rules>
auto card_score = [](char c) -> int {
    switch (c) {
    case 'A': return 14;
    case 'K': return 13;
    case 'Q': return 12;
    case 'J': return Rules == rules::with_jokers ? 1 : 11;
    case 'T': return 10;
    default:
        assert(c >= '2' && c <= '9');
        return c - '0';
    }
};

template <rules Rules>
auto compare_hands = [](hand_t const& lhs, hand_t const& rhs) -> std::strong_ordering
{
    auto cmp = evaluate_hand<Rules>(lhs) <=> evaluate_hand<Rules>(rhs);

    if (cmp != 0) {
        return cmp;
    } else {
        return flux::compare(lhs, rhs, flux::proj(std::strong_order, card_score<Rules>));
    }
};

template <rules Rules>
auto calculate_score = [](std::vector<std::pair<hand_t, int>> input) -> int64_t
{
    flux::sort(input, [](auto const& lhs, auto const& rhs) {
        return std::is_lt(compare_hands<Rules>(lhs.first, rhs.first));
    });

    return flux::zip(flux::ints(1), flux::ref(input).map(&std::pair<hand_t, int>::second))
        .map(flux::unpack(std::multiplies{}))
        .sum();
};

auto part1 = calculate_score<rules::no_jokers>;
auto part2 = calculate_score<rules::with_jokers>;

constexpr auto& test_data =
R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483)";

static_assert([]{
    auto pairs = parse_input(test_data);
    return part1(pairs) == 6440
        && part2(pairs) == 5905;
}());

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const hands_and_bids = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(hands_and_bids));
    fmt::println("Part 2: {}", part2(hands_and_bids));
}