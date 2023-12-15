
#include "../aoc.hpp"

namespace {

auto hash_string = [](std::string_view str) -> int
{
    return flux::fold(str, [](int hash, char c) {
               hash += c;
               hash *= 17;
               hash %= 256;
               return hash;
           });
};

auto part1 = [](std::string_view input) -> int
{
    return flux::split_string(input, ',').map(hash_string).sum();
};

struct lens {
    std::string label;
    int focal_length;
};

auto part2 = [](std::string_view input) -> int
{
    std::array<std::vector<lens>, 256> boxes;

    // Arrange all the boxes
    for (std::string_view str : flux::split_string(input, ',')) {
        auto const op = str.find_first_of("=-");
        auto label = str.substr(0, op);
        auto& box = boxes.at(hash_string(label));
        auto iter = std::ranges::find(box, label, &lens::label);

        if (str.at(op) == '-') {
            if (iter != box.end()) {
                box.erase(iter);
            }
        } else if (str.at(op) == '=') {
            int len = aoc::parse<int>(str.substr(op + 1));

            if (iter != box.end()) {
                iter->focal_length = len;
            } else {
                box.emplace_back(std::string(label), len);
            }
        }
    }

    // Calculate the score
    int score = 0;

    for (auto const& [box, box_num] : flux::zip(flux::ref(boxes), flux::ints(1))) {
        for (auto const& [lens, slot_num] : flux::zip(flux::ref(box), flux::ints(1))) {
            score += box_num * slot_num * lens.focal_length;
        }
    }

    return score;
};

constexpr auto& test_data = "rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7";

static_assert(hash_string("HASH") == 52);
static_assert(part1(test_data) == 1320);
static_assert(part2(test_data) == 145);

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