
#include "../aoc.hpp"

namespace {

using i64 = std::int64_t;

struct map_entry {
    i64 dest_start;
    i64 source_start;
    i64 length;
};

using mapping = std::vector<map_entry>;

using maps_t = std::array<mapping, 7>;

auto str_to_i64 = [](std::string_view str) {
    return aoc::try_parse<i64>(str).value();
};

auto parse_seeds = [](std::string_view line) -> std::vector<i64>
{
    assert(line.starts_with("seeds: "));
    line.remove_prefix(7);

    return flux::split_string(line, ' ').map(str_to_i64).to<std::vector>();
};

auto parse_map = [](std::string_view block) -> mapping
{
    return flux::split_string(block, '\n')
            .filter([](auto sv) { return !sv.empty(); })
            .drop(1) // "x-to-x map:"
            .map([](std::string_view line) -> map_entry {
                auto nums = flux::split_string(line, ' ').map(str_to_i64);
                auto idx = nums.first();
                return map_entry{
                    .dest_start = nums[idx],
                    .source_start = nums[nums.inc(idx)],
                    .length = nums[nums.inc(idx)]
                };
             })
            .to<mapping>();
};

auto parse_input = [](std::string_view input) -> std::pair<std::vector<i64>, maps_t>
{
    auto blocks = flux::split_string(input, "\n\n");

    auto seeds = parse_seeds(blocks.front().value());

    // It really feels like I should have a nicer way of doing this
    maps_t maps{};
    flux::drop(blocks, 1).map(parse_map).output_to(maps.begin());

    return std::pair(std::move(seeds), std::move(maps));
};

auto part1 = [](std::vector<i64> const& seeds, maps_t const& maps) -> i64
{
    return flux::ref(seeds)
            .map([&maps](i64 seed) -> i64 {
                 for (mapping const& map : maps) {
                     for (map_entry const& e : map) {
                         i64 offset = seed - e.source_start;
                         if (offset >= 0 && offset < e.length) {
                             seed = e.dest_start + offset;
                             break;
                         }
                     }
                 }
                 return seed;
             })
            .min()
            .value();
};

struct seed_range {
    i64 start;
    i64 length;
};

auto part2 = [](std::vector<i64> const& seeds, maps_t const& maps) -> i64
{
    auto ranges = flux::ref(seeds)
                      .pairwise()
                      .stride(2)
                      .map(flux::unpack([](i64 a, i64 b) { return seed_range{a, b}; }))
                      .to<std::vector>();
#ifdef USE_OPENMP
    std::vector<i64> mins(ranges.size());

#pragma omp parallel for
    for (size_t i = 0; i < mins.size(); ++i) {
        seed_range r = ranges.at(i);
        mins.at(i) = flux::iota(r.start, r.start + r.length)
            .map([&maps](i64 seed) -> i64 {
                for (mapping const& map : maps) {
                    for (map_entry const& e : map) {
                        i64 offset = seed - e.source_start;
                        if (offset >= 0 && offset < e.length) {
                            seed = e.dest_start + offset;
                            break;
                        }
                    }
                }
                return seed;
            })
            .min().value();
    }
    return flux::min(mins).value();
#else
    return flux::ref(ranges)
            .map([&maps](seed_range r) {
                return flux::iota(r.start, r.start + r.length)
                        .map([&maps](i64 seed) -> i64 {
                            for (mapping const& map : maps) {
                                for (map_entry const& e : map) {
                                    i64 offset = seed - e.source_start;
                                    if (offset >= 0 && offset < e.length) {
                                        seed = e.dest_start + offset;
                                        break;
                                    }
                                }
                            }
                            return seed;
                        })
                        .min().value();
             })
            .min()
            .value();
#endif
};


constexpr auto& test_data =
R"(seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4)";

}

int main(int argc, char** argv)
{
    {
        if (argc < 2) {
            fmt::println(stderr, "No input");
            return -1;
        }
    }

    {
        auto const [seeds, maps] = parse_input(test_data);
        assert(part1(seeds, maps) == 35);
        assert(part2(seeds, maps) == 46);
    }

    auto const [seeds, maps] = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(seeds, maps));
    fmt::println("Part 2: {}", part2(seeds, maps));

}