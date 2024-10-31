
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

template <typename T>
struct vec3_t {
    T x, y, z;

    friend auto operator==(vec3_t const&, vec3_t const&) -> bool = default;

    friend constexpr auto operator+(vec3_t const& lhs, vec3_t const& rhs) -> vec3_t
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    friend constexpr auto operator-(vec3_t const& lhs, vec3_t const& rhs) -> vec3_t
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    friend constexpr auto operator*(T const& s, vec3_t const& vec) -> vec3_t
    {
        return {s * vec.x, s * vec.y, s * vec.z};
    }
};

template <typename T>
constexpr auto dot(vec3_t<T> const& a, vec3_t<T> const& b) -> T
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

template <typename T>
constexpr auto cross(vec3_t<T> const& a, vec3_t<T> const& b) -> vec3_t<T>
{
    return {.x = (a.y * b.z) - (a.z * b.y),
            .y = (a.z * b.x) - (a.x * b.z),
            .z = (a.x * b.y) - (a.y * b.x)};
}

using dvec3 = vec3_t<double>;

struct hailstone {
    dvec3 position;
    dvec3 velocity;

    auto operator==(hailstone const&) const -> bool = default;
};

auto parse_vec3 = [](std::string_view str) -> dvec3 {
    auto seq =  flux::split_string(str, ',')
                   .filter(std::not_fn(flux::is_empty))
                   .map(aoc::parse<i64>);
    auto pos = seq.first();
    return dvec3{
        .x = double(seq[pos]), .y = double(seq[seq.inc(pos)]), .z = double(seq[seq.inc(pos)])
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
    auto a = h1.velocity.y/h1.velocity.x;
    auto b = h2.velocity.y/h2.velocity.x;

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

// This solution was entirely taken from a Reddit post by user /u/DaveBaum. Thanks, Dave!
// https://www.reddit.com/r/adventofcode/comments/18pnycy/2023_day_24_solutions/kxqjg33/
auto part2 = [](std::vector<hailstone> const& input) -> i64
{
    auto& h0 = input.at(0);
    auto& h1 = input.at(1);
    auto& h2 = input.at(2);

    auto p1 = h1.position - h0.position;
    auto v1 = h1.velocity - h0.velocity;
    auto p2 = h2.position - h0.position;
    auto v2 = h2.velocity - h0.velocity;

    auto t1 = -dot(cross(p1, p2), v2)/dot(cross(v1, p2), v2);
    auto t2 = -dot(cross(p1, p2), v1)/dot(cross(p1, v2), v1);

    auto c1 = h1.position + (t1 * h1.velocity);
    auto c2 = h2.position + (t2 * h2.velocity);

    auto v = (1.0/(t2 - t1)) *  (c2 - c1);
    auto p = c1 - (t1 * v);

    return i64(p.x + p.y + p.z);
};

constexpr auto& test_data =
R"(19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3)";

static_assert([]{
    auto const stones = parse_input(test_data);
    return count_intersections<7, 27>(stones) == 2 &&
           part2(stones) == 47;
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
    fmt::println("Part 2: {}", part2(stones));
}