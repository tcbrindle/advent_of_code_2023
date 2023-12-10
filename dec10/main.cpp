
#include "../aoc.hpp"

namespace {

using namespace std::string_view_literals;

enum class direction {
    north, south, east, west
};

struct position {
    int x, y;

    constexpr bool operator==(position const&) const = default;
    constexpr auto operator<=>(position const&) const = default;

    friend constexpr auto operator+(position p, direction d) -> position {
        switch (d) {
        case direction::north: --p.y; break;
        case direction::south: ++p.y; break;
        case direction::east: ++p.x; break;
        case direction::west: --p.x; break;
        }
        return p;
    }
};

struct grid_t {
    std::string tiles;
    int size; // assumed square

    constexpr auto operator[](position pos) const -> char
    {
        if (pos.x >= 0 && pos.y >= 0 && pos.x < size && pos.y < size) {
            return tiles[pos.y * size + pos.x];
        } else {
            return '.';
        }
    }

    constexpr auto idx_to_pos(int idx) const -> position
    {
        return {.x = idx % size, .y = idx/size};
    }

    constexpr auto pos_to_idx(position pos) const -> int
    {
        return pos.y * size + pos.x;
    }
};

auto parse_input = [](std::string_view input) -> grid_t
{
    return grid_t{
        .tiles = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .size = static_cast<int>(input.find('\n'))
    };
};

auto tile_to_directions = [](char tile) -> std::pair<direction, direction>
{
    using enum direction;
    switch (tile) {
    case '|': return {north, south};
    case '-': return {east, west};
    case 'L': return {north, east};
    case 'J': return {north, west};
    case '7': return {south, west};
    case 'F': return {south, east};
    default: throw std::runtime_error(fmt::format("Unknown tile kind {}", tile));
    }
};

auto find_starting_direction = [](grid_t const& grid, position pos) -> direction
{
    constexpr std::array<std::pair<std::string_view, direction>, 3> table{{
        {"|7F", direction::north}, {"-J7", direction::east},
        {"|LJ", direction::south}
    }};
    for (auto [str, dir] : table) {
        if (str.contains(grid[pos + dir])) {
            return dir;
        }
    }
    throw std::runtime_error("Could not find a starting direction!");
};

auto path_sequence = [](grid_t const& grid) -> flux::sequence auto
{
    auto generate_fn = [&grid](position pos, position prev) {
        // Find the two directions we can go in from this tile
        auto [next_dir1, next_dir2] = tile_to_directions(grid[pos]);
        // One of them leads where we just came from, so choose the other
        auto dir = pos + next_dir1 != prev ? next_dir1 : next_dir2;
        return std::pair{pos + dir, pos};
    };

    position start_pos = grid.idx_to_pos(grid.tiles.find('S'));
    direction start_dir = find_starting_direction(grid, start_pos);

    return flux::unfold(flux::unpack(generate_fn),
                        std::pair{start_pos + start_dir, start_pos})
                .take_while([&grid](auto p) { return grid[p.first] != 'S'; })
                .map(&std::pair<position, position>::first);
};

auto part1 = [](grid_t const& grid) -> int
{
    return 1 + path_sequence(grid).count()/2;
};

auto part2 = [](grid_t grid) -> int
{
    std::vector<position> path = path_sequence(grid).to<std::vector>();

    path.push_back(grid.idx_to_pos(grid.tiles.find('S')));
    flux::sort(path);
    grid.tiles.at(grid.tiles.find('S')) = 'F';

    int enclosed_count = 0;

    for (int y : flux::iota(0, grid.size)) {
        bool inside = false;
        for (int x = 0; x < grid.size; ++x) {
            if (std::ranges::binary_search(path, position{x, y})) {
                char const tile = grid[{x, y}];
                int const idx = grid.pos_to_idx({x, y});

                if (tile == '|') {
                    inside = !inside;
                } else if (tile == 'L' || tile == 'F') {
                    auto nxt = grid.tiles.find_first_not_of('-', 1 + idx);
                    x += (nxt - idx);
                    char const nxt_tile = grid[{x, y}];
                    if ((tile == 'L' && nxt_tile == '7') ||
                        (tile == 'F' && nxt_tile == 'J')) {
                        inside = !inside;
                    }
                }
            } else {
                enclosed_count += inside;
            }
        }
    }

    return enclosed_count;
};

constexpr auto& test_data1 =
R"(.....
.S-7.
.|.|.
.L-J.
.....)";

constexpr auto& test_data2 =
R"(-L|F7
7S-7|
L|7||
-L-J|
L|-JF)";

constexpr auto& test_data3 =
R"(..F7.
.FJ|.
SJ.L7
|F--J
LJ...)";

constexpr auto& test_data4 =
R"(7-F7-
.FJ|7
SJLL7
|F--J
LJ.LJ)";

constexpr auto& test_data5 =
R"(...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........)";

constexpr auto& test_data6 =
R"(..........
.S------7.
.|F----7|.
.||OOOO||.
.||OOOO||.
.|L-7F-J|.
.|II||II|.
.L--JL--J.
..........)";

constexpr auto& test_data7 =
R"(.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...)";

constexpr auto test_data8 =
R"(FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L)";

}

int main(int argc, char** argv)
{
    {
        static_assert(part1(parse_input(test_data1)) == 4);
        static_assert(part1(parse_input(test_data2)) == 4);
        static_assert(part1(parse_input(test_data3)) == 8);
        static_assert(part1(parse_input(test_data4)) == 8);

        static_assert(part2(parse_input(test_data5)) == 4);
        static_assert(part2(parse_input(test_data6)) == 4);
        static_assert(part2(parse_input(test_data7)) == 8);
        static_assert(part2(parse_input(test_data8)) == 10);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    grid_t input = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}