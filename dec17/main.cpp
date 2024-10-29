
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

template <typename G>
concept Graph =
    std::regular<typename G::node_type> &&
    std::regular<typename G::distance_type> &&
    std::totally_ordered<typename G::distance_type> &&
    requires (G const& g, typename G::node_type n) {
        { g.neighbours(n) } -> flux::sequence; // of pair<node_type, distance_type>
        { g.should_exit(n) } -> std::same_as<bool>;
    };

auto dijkstra =
[]<Graph G>(G const& graph, G::node_type start)
    -> std::map<typename G::node_type, typename G::distance_type>
{
    using node_t = G::node_type;
    using dist_t = G::distance_type;
    using queue_t = std::priority_queue<
                        std::pair<dist_t, node_t>,
                        std::vector<std::pair<dist_t, node_t>>,
                        std::greater<>>;
    using map_t = std::map<node_t, dist_t>;

    queue_t queue{};
    map_t dists;

    queue.push({0, start});
    dists[start] = dist_t{};

    while (!queue.empty()) {
        auto [_, current] = queue.top();
        queue.pop();

        if (graph.should_exit(current)) {
            break;
        }

        dist_t current_dist= dists.find(current)->second;

        for (auto const& [next_node, next_dist] : graph.neighbours(current)) {
            dist_t new_dist = dists[current] + next_dist;
            if (auto iter = dists.find(next_node); iter == dists.cend() || new_dist < iter->second) {
                dists[next_node] = new_dist;
                queue.push({new_dist, next_node});
            }
        }
    }

    return dists;
};

template <i64 MinDist, i64 MaxDist>
struct crucible_graph {
    grid2d const& grid;

    struct node_type {
        position pos{};
        flux::optional<direction> dir;

        bool operator==(node_type const&) const = default;
        auto operator<=>(node_type const&) const = default;
    };

    using distance_type = i64;

    auto neighbours(node_type const& n) const
       -> std::vector<std::pair<node_type, distance_type>>
    {
        std::vector<std::pair<node_type, distance_type>> out;

        auto next_dirs =
            flux::ints(0, 4)
              .filter([&](i64 d) {
                if (n.dir.has_value()) {
                    return flux::pred::even(d) != flux::pred::even(int(*n.dir));
                } else {
                    return true;
                }
              })
              .map([](i64 d) { return static_cast<direction>(d); });

        for (direction next_dir : next_dirs) {
            auto next_pos = n.pos;
            auto cost = 0;

            for (auto i : flux::ints(1, MaxDist + 1)) {
                next_pos = next_pos + next_dir;
                if (!grid.in_bounds(next_pos)) {
                    break;
                }
                cost += grid[next_pos];
                if (i >= MinDist) {
                    out.emplace_back(
                        node_type{.pos = next_pos, .dir = flux::optional(next_dir)}, cost);
                }
            }
        }

        return out;
    }

    auto should_exit(node_type const& n) const -> bool
    {
        return n.pos == position{grid.width - 1, grid.height - 1};
    }
};

template <int MinDist, int MaxDist>
auto calculate = [](grid2d const& grid) -> i64
{
    crucible_graph<MinDist, MaxDist> graph{grid};

    auto dists = dijkstra(graph, {.pos = {0, 0}});

    return flux::from_crange(dists)
        .filter([&grid](auto const& pair) {
            return pair.first.pos == position{grid.width - 1, grid.height - 1};
        })
        .map([](auto const& pair) { return pair.second; })
        .min()
        .value();
};

auto const part1 = calculate<1, 3>;
auto const part2 = calculate<4, 10>;

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
4322674655533
)";

constexpr auto& test_data2 =
R"(111111111111
999999999991
999999999991
999999999991
999999999991
)";

}

int main(int argc, char** argv)
{
    {
        auto const test_grid = parse_input(test_data);
        fmt::println("Part 1 test: {}", part1(test_grid));
        fmt::println("Part 2 test: {}", part2(test_grid));

        auto const test_grid2 = parse_input(test_data2);
        fmt::println("Part 2 test2: {}", part2(test_grid2));
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(grid));
    fmt::println("Part 2: {}", part2(grid));
}