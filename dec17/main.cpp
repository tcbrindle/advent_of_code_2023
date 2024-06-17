
#include "../aoc.hpp"

#include <queue>
#include <set>

namespace {

using i64 = std::int64_t;

enum class direction : std::uint8_t {
    north, east, south, west
};

struct position {
    i64 x, y;

    bool operator==(position const&) const = default;
    auto operator<=>(position const&) const = default;

    friend constexpr auto operator+(position p, direction d) -> position
    {
        switch (d){
        case direction::north: --p.y; break;
        case direction::east: ++p.x; break;
        case direction::south: ++p.y; break;
        case direction::west: --p.x; break;
        }
        return p;
    }
};

struct grid2d {
    std::vector<int> data;
    i64 width;
    i64 height;

    constexpr auto operator[](position pos) const -> int
    {
        return data.at(pos.y * width + pos.x);
    }

    constexpr auto in_bounds(position pos) const -> bool
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }
};

auto parse_input = [](std::string_view input) -> grid2d
{
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n'))
                    .map([](char c) { return c - '0'; })
                    .to<std::vector>(),
        .width = flux::find(input, '\n'),
        .height = flux::count_eq(input, '\n')
    };
};

auto get_neighbours = [](grid2d const& grid, position const pos, direction const dir)
    -> flux::sequence auto
{
    std::vector<std::tuple<position, direction, int>> out;
    for (uint8_t d : flux::ints(0, 4)) {
        if (flux::pred::even(d) == flux::pred::even(int(dir))) {
            continue;
        }

        direction n{d};

        for (int i : flux::ints(1, 4)) {
            int weight = 0;
            auto next_pos = pos;
            bool in_bounds = true;

            for (int k : flux::ints(1, i + 1)) {
                next_pos = next_pos + n;
                if (!grid.in_bounds(next_pos)) {
                    in_bounds = false;
                    break;
                }

                weight += grid[pos];
            }

            if (in_bounds) {
                out.push_back({next_pos, n, weight});
            }
        }
    }

    return out;
};

struct State {
    position pos;
    direction dir;
    bool operator==(State const&) const = default;
    auto operator<=>(State const&) const = default;
};

auto part1 = [](grid2d const& grid) -> int
{
    std::priority_queue<std::pair<int, State>,
                        std::vector<std::pair<int, State>>,
                        std::greater<>> queue;

    queue.push({0, State{position{0, 0}, direction::south}});
    queue.push({0, State{position{0, 0}, direction::east}});

    std::map<State, int> dists;
    dists[{{0, 0}, direction::south}] = 0;
    dists[{{0, 0}, direction::east}] = 0;

    while (!queue.empty()) {
        auto const [_, state] = queue.top();
        queue.pop();

        if (state.pos == position{grid.width - 1, grid.height - 1}) {
            break;
        }

        for (auto [next_pos, next_dir, next_weight] : get_neighbours(grid, state.pos, state.dir)) {

            auto next_dist = dists[state] + next_weight;
            auto next_state = State{next_pos, next_dir};

            if (auto iter = dists.find(next_state);
                iter == dists.end() || next_dist < iter->second) {
                dists[next_state] = next_dist;
                queue.push({next_dist, next_state});
            }
        }
    }

    auto iter = std::ranges::find_if(dists, [&grid](auto const& pair) {
        return pair.first.pos == position{grid.width - 1, grid.height - 1};
    });
    return iter->second;
};
#if 0
auto part2 = [](grid2d const& grid) -> int
{
    std::priority_queue<std::pair<int, State>,
                        std::vector<std::pair<int, State>>,
                        std::greater<>> queue;

    queue.push({0, State{position{0, 0}, direction{99}, 0}});
    std::map<State, int> dists;
    dists[{{0, 0}, direction{99}, 0}] = 0;

    while (!queue.empty()) {
        auto const [_, state] = queue.top();
        queue.pop();

        for (auto [next_pos, next_dir] : get_neighbours(grid, state.pos)) {
            int consec = next_dir == state.dir ? state.consec + 1 : 0;
            if (consec >= 10) {
                continue;
            }

            auto next_dist = dists[state] + grid[next_pos];
            auto next_state = State{next_pos, next_dir, consec};

            if (auto iter = dists.find(next_state);
                iter == dists.end() || next_dist < iter->second) {
                dists[next_state] = next_dist;
                queue.push({next_dist, next_state});
            }
        }
    }

    auto iter = std::ranges::find_if(dists, [&grid](auto const& pair) {
        return pair.first.pos == position{grid.width - 1, grid.height - 1} &&
               pair.first.consec >= 0;
    });
    return iter->second;
};
#endif

constexpr auto& test_data =
R"(2413432311323
3215453535623
3255245654254
3446585845452
4546657867536
1438598798454
4457876987766
3637877979653
4654967986887
4564679986453
1224686865563
2546548887735
4322674655533)";

constexpr auto& test_data2 =
R"(111111111111
999999999991
999999999991
999999999991
999999999991)";

}

int main(int argc, char** argv)
{
    {
        auto const test_grid = parse_input(test_data);
        fmt::println("Part 1 test: {}", part1(test_grid));
    //    fmt::println("Part 2 test: {}", part2(test_grid));
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(grid));
  //  fmt::println("Part 2: {}", part2(grid));
}