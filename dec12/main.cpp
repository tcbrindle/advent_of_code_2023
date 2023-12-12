
#include "../aoc.hpp"

namespace {

using i64 = int64_t;

struct row {
    std::string record;
    std::vector<int> counts;
};

auto parse_input = [](std::string_view input) -> std::vector<row>
{
    return flux::split_string(input, '\n')
            .filter([](std::string_view line) { return !line.empty(); })
            .map([](std::string_view line) -> row {
                   auto sp = line.find(' ');
                   return row {
                       .record = std::string(line.substr(0, sp)),
                       .counts = flux::split(line.substr(sp+1), ',')
                                    .map(aoc::parse<int>)
                                    .to<std::vector>()
                   };
             })
            .to<std::vector>();
};

// Urgh, why doesn't C++ provide a hash specialisation for tuple? :(
using cache_t = std::map<std::tuple<int, int, int>, i64>;

auto analyse_row_recursive(std::string const& record,
                           std::vector<int> const& groups,
                           cache_t& cache,
                           int record_idx,
                           int hash_count,
                           int group_idx) -> i64
{
    if (auto iter = cache.find({record_idx, hash_count, group_idx}); iter != cache.end()) {
        return iter->second;
    }

    // We have reached the end of this record
    if (record_idx == record.size()) {
        // We have no remaining groups and are not in a run of #s -- success!
        if (hash_count == 0 && group_idx == groups.size()) {
            return 1;
        // We have one remaining group, and it's the same size as our run of #s -- success!
        } else if (group_idx == groups.size() - 1 && groups.at(group_idx) == hash_count) {
            return 1;
        // In any other case, this is not a valid permutation
        } else{
            return 0;
        }
    }

    i64 total = 0;

    char const c = record.at(record_idx);

    // We're starting or continuing a streak of #s
    if (c == '#' || c == '?') {
        total += analyse_row_recursive(record, groups, cache, record_idx + 1, hash_count + 1, group_idx);
    }

    if (c == '.' || c == '?') {
        // We're not on a streak, so just bump the record idx and continue
        if (hash_count == 0) {
            total += analyse_row_recursive(record, groups, cache, record_idx + 1, 0, group_idx);
        // We're at the end of a streak and it's the right size, so move on to
        // the next group
        } else if (group_idx < groups.size() && groups.at(group_idx) == hash_count) {
            total += analyse_row_recursive(record, groups, cache, record_idx + 1, 0, group_idx + 1);
        }
    }

    cache[{record_idx, hash_count, group_idx}] = total;

    return total;
}


auto analyse_row = [](row const& r) -> i64
{
    cache_t cache;
    return analyse_row_recursive(r.record, r.counts, cache, 0, 0, 0);
};

auto part1 = [](std::vector<row> const& input) -> i64
{
    return flux::ref(input).map(analyse_row).sum();
};


auto part2 = [](std::vector<row> input) -> i64
{
    for (auto& [record, counts] : input) {
        // Wah, I need join_with :(
        auto r = flux::ref(record);
        auto s = flux::single('?');
        record = flux::chain(r, s, r, s, r, s, r, s, r).to<std::string>();

        counts = flux::ref(counts).cycle(5).to<std::vector>();
    }

    std::vector<i64> totals(input.size());

#pragma omp parallel for
    for (size_t i = 0; i < input.size(); i++) {
        totals.at(i) = analyse_row(input.at(i));
    }

    return flux::sum(totals);
};

constexpr auto& test_data1 =
R"(#.#.### 1,1,3
.#...#....###. 1,1,3
.#.###.#.###### 1,3,1,6
####.#...#... 4,1,1
#....######..#####. 1,6,5
.###.##....# 3,2,1)";

constexpr auto& test_data2 =
R"(???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1)";

}

int main(int argc, char** argv)
{
    assert(part1(parse_input(test_data1)) == 6);
    assert(part1(parse_input(test_data2)) == 21);
    assert(part2(parse_input(test_data2)) == 525152);

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto input = parse_input(aoc::string_from_file(argv[1]));
    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}