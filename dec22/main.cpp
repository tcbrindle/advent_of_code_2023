
#include "../aoc.hpp"

namespace {

using vec3 = std::array<int, 3>;

// Don't do this at home, kids
constexpr int x = 0;
constexpr int y = 1;
constexpr int z = 2;

struct brick_t {
    vec3 from, to;
};

auto parse_input = [](std::string_view input) -> std::vector<brick_t>
{
    auto to_vec3 = [](std::string_view str) -> vec3 {
        vec3 out;
        flux::split_string(str, ',').map(aoc::parse<int>).output_to(out.begin());
        return out;
    };

    return flux::split_string(input, '\n')
            .filter(std::not_fn(flux::is_empty))
            .map([&](std::string_view line) -> brick_t {
                auto tilde = line.find('~');
                return brick_t{
                    .from = to_vec3(line.substr(0, tilde)),
                    .to = to_vec3(line.substr(tilde + 1))
                };
            })
            .to<std::vector>();
};

auto has_overlap = [](brick_t const& lhs, brick_t const& rhs, auto dim) -> bool
{
    int l_min = lhs.from.at(dim);
    int l_max = lhs.to.at(dim);
    if (l_max < l_min) {
        std::swap(l_min, l_max);
    }

    int r_min = rhs.from.at(dim);
    int r_max = rhs.to.at(dim);
    if (r_max < r_min) {
        std::swap(r_min, r_max);
    }

    // Disjoint if l_max < r_min, or r_max < l_min
    return !((l_max < r_min) || (r_max < l_min));
};

auto has_xy_overlap = [](brick_t const& lhs, brick_t const& rhs) -> bool
{
    return has_overlap(lhs, rhs, x) && has_overlap(lhs, rhs, y);
};

auto rests_on = [](brick_t const& lhs, brick_t const& rhs) -> bool
{
    return (lhs.to.at(z) + 1 == rhs.from.at(z)) && has_xy_overlap(lhs, rhs);
};

auto run_gravity = [](std::vector<brick_t>& bricks) -> int
{
    int fall_count = 0;
    flux::zip(flux::mut_ref(bricks), flux::ints())
        .filter([](auto pair) { return pair.first.from.at(z) > 1; })
        .for_each([&](auto pair) {
            auto& [brick, idx] = pair;
            if (flux::ints(0, idx).none([&](auto next_idx) {
                    return rests_on(bricks.at(next_idx), brick); })) {
                --brick.from.at(z);
                --brick.to.at(z);
                ++fall_count;
            }
        });
    return fall_count;
};

auto prepare_bricks = [](std::vector<brick_t>& bricks) -> void
{
    // Make sure each brick's from.z is less than its to.z
    flux::mut_ref(bricks).for_each([](brick_t& brick) {
        if (brick.to.at(z) < brick.from.at(z)) {
            std::swap(brick.from, brick.to);
        }
        assert(brick.from.at(z) <= brick.to.at(z));
    });

    // Sort the bricks by their z coordinates
    flux::sort(bricks, [](brick_t const& lhs, brick_t const& rhs) {
        return lhs.from.at(z) < rhs.from.at(z);
    });

    // For each brick, if its z coordinate is greater than one, check what is
    // immediately below it and if there is nothing there, move it down one
    // unit. Repeat until no more blocks move.
    while (run_gravity(bricks) != 0) {}

    // Re-sort the bricks by z coord in case this has changed
    flux::sort(bricks, [](brick_t const& lhs, brick_t const& rhs) {
        return lhs.from.at(z) < rhs.from.at(z);
    });
};

auto part1 = [](std::vector<brick_t> bricks) -> int
{
    // for each brick A...
    return flux::ints(0, flux::size(bricks))
            // we can eliminate it if...
            .count_if([&](auto idx) {
                // for each brick B above it...
                return flux::ints(idx, flux::size(bricks))
                        // if A rests on B...
                        .filter([&](auto next_idx) {
                           return rests_on(bricks.at(idx), bricks.at(next_idx));
                        })
                        // ... then count how many bricks B rests on
                        .map([&](auto next_idx) {
                           return flux::ints(0, next_idx)
                                    .reverse()
                                    .count_if([&](auto prev_idx) {
                                       return rests_on(bricks.at(prev_idx), bricks.at(next_idx));
                                    });
                        })
                        // ...and we can eliminate A if all Bs rest on at least two bricks
                        .all(flux::pred::geq(2));
            });
};

auto part2 = [](std::vector<brick_t> bricks) -> int64_t
{
    // for each brick, eliminate it from the bricks array and re-run gravity,
    // to see what falls
    return flux::ints(0, flux::size(bricks))
            .map([&](auto idx) {
                   auto bricks_copy = flux::copy(bricks);
                   bricks_copy.erase(bricks_copy.begin() + idx);
                   return run_gravity(bricks_copy);
             })
            .sum();
};

constexpr auto test_data =
R"(1,0,1~1,2,1
0,0,2~2,0,2
0,2,3~2,2,3
0,0,4~0,2,4
2,0,5~2,2,5
0,1,6~2,1,6
1,1,8~1,1,9)";

}

int main(int argc, char** argv)
{
    {
        auto bricks = parse_input(test_data);
        prepare_bricks(bricks);
        fmt::println("Part 1 test: {}", part1(bricks));
        fmt::println("Part 2 test: {}", part2(bricks));
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto bricks = parse_input(aoc::string_from_file(argv[1]));
    prepare_bricks(bricks);

    fmt::println("Part 1: {}", part1(bricks));
    fmt::println("Part 2: {}", part2(bricks));
}
