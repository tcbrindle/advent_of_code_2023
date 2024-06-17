
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

struct vec3 {
    i64 x, y, z;

    auto operator==(vec3 const&) const -> bool = default;
};

struct hailstone {
    vec3 position;
    vec3 velocity;

    auto operator==(hailstone const&) const -> bool = default;
};

auto parse_vec3 = [](std::string_view str) -> vec3 {
    auto seq =  flux::split_string(str, ',')
                   .filter(std::not_fn(flux::is_empty))
                   .map(aoc::parse<i64>);
    auto pos = seq.first();
    return vec3{
        .x = seq[pos], .y = seq[seq.inc(pos)], .z = seq[seq.inc(pos)]
    };
};

auto parse_input = [](std::string_view input) -> std::vector<hailstone>
{
    return flux::split_string(input, '\n')
            .filter(std::not_fn(flux::is_empty))
            .map([](std::string_view line) -> hailstone {
                auto at = line.find('@');
                return {.position = parse_vec3(line.substr(0, at - 1)),
                        .velocity = parse_vec3(line.substr(at + 1))};
            })
            .to<std::vector>();
};

template <i64 Min, i64 Max>
auto will_intersect = [](hailstone const& h1, hailstone const& h2) -> bool
{
    auto a = double(h1.velocity.y)/h1.velocity.x;
    auto b = double(h2.velocity.y)/h2.velocity.x;

    if (a == b) {
        // Lines are parallel
        return false;
    }

    auto c = h1.position.y - (h1.position.x * a);
    auto d = h2.position.y - (h2.position.x * b);

    auto px = (d - c)/(a - b);
    auto py = c + (a * px);

    auto t1 = (px - h1.position.x)/h1.velocity.x;
    auto t2 = (px - h2.position.x)/h2.velocity.x;

    if (t1 < 0 || t2 < 0) {
        // In the past
        return false;
    }

    return px >= Min && px <= Max && py >= Min && py <= Max;
};

template <i64 Min, i64 Max>
auto count_intersections = [](std::vector<hailstone> const& input) -> i64
{
    i64 count = 0;
    for (auto i : flux::ints(0, flux::size(input))) {
        for (auto j : flux::ints(i + 1, flux::size(input))) {
            if (will_intersect<Min, Max>(input.at(i), input.at(j))) {
                ++count;
            }
        }
    }
    return count;
};

auto part1 = count_intersections<200000000000000, 400000000000000>;


constexpr auto& test_data =
R"(19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3)";

static_assert([]{
    auto const stones = parse_input(test_data);
    return count_intersections<7, 27>(stones) == 2;
}());

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const stones = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(stones));
}