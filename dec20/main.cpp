
#include "../aoc.hpp"

#include <numeric>
#include <queue>

namespace {

enum class pulse_kind : bool { lo, hi };

enum class module_kind {
    flipflop, conjunction, broadcast
};

struct module {
    template <typename Messenger>
    void on_pulse(Messenger& m, pulse_kind p, std::string const& from)
    {
        if (kind == module_kind::flipflop) {
             if (p == pulse_kind::lo) {
                if (!state) {
                    state = true;
                    m.send(pulse_kind::hi, name, dests);
                } else {
                    state = false;
                    m.send(pulse_kind::lo, name, dests);
                }
            }
        } else if (kind == module_kind::conjunction) {
            inputs[from] = p;
            if (flux::from_crange(inputs | std::views::values)
                    .all(flux::pred::eq(pulse_kind::hi))) {
                m.send(pulse_kind::lo, name, dests);
            } else {
                m.send(pulse_kind::hi, name, dests);
            }
        } else if (kind == module_kind::broadcast) {
            m.send(p, name, dests);
        }
    }

    module_kind kind;
    std::string name;
    std::vector<std::string> dests;
    bool state = false;
    std::unordered_map<std::string, pulse_kind> inputs;
};

using module_map = std::unordered_map<std::string, module>;

auto parse_input = [](std::string_view input) -> module_map
{
    module_map map =
        flux::split_string(input, '\n')
        .filter(std::not_fn(flux::is_empty))
        .map([](std::string_view line) -> std::pair<std::string, module> {
            auto kind = [&] {
                if (line[0] == '%') {
                    line.remove_prefix(1); return module_kind::flipflop;
                } else if (line[0] == '&') {
                    line.remove_prefix(1); return module_kind::conjunction;
                } else {
                    return module_kind::broadcast;
                }
            }();

            module mod{.kind = kind};
            auto arrow = line.find(" -> ");
            mod.name = std::string(line.substr(0, arrow));
            mod.dests = flux::split_string(line.substr(arrow + 4), ", ")
                            .to<std::vector<std::string>>();
            return {mod.name, std::move(mod)};
        })
        .to<module_map>();

    // Let each module know about its inputs
    for (auto const& [name, module] : map) {
        for (std::string const& dest_name : module.dests) {
            if (auto iter = map.find(dest_name); iter != map.end()) {
                iter->second.inputs[name] = pulse_kind::lo;
            }
        }
    }

    return map;
};

struct messenger {
    struct message_info {
        pulse_kind p;
        std::string sender;
        std::string dest;
    };

    void send(pulse_kind p, std::string const& sender,
              std::vector<std::string> const& dests)
    {
        for (const auto& d : dests) {
            msg_q.push({p, sender, d});
        }
    }

    bool run(module_map& map, std::string const& target = "")
    {
        msg_q.push({pulse_kind::lo, "button", "broadcaster"});
        while (!msg_q.empty()) {
            auto info = msg_q.front();
            msg_q.pop();

            if (info.p == pulse_kind::hi) {
                ++hi_count;
            } else {
                ++lo_count;
            }

            if (info.p == pulse_kind::lo && info.dest == target) {
                return true;
            }

            if (!map.contains(info.dest)) {
                continue;
            }

            map[info.dest].on_pulse(*this, info.p, info.sender);
        }
        return false;
    }

    std::queue<message_info> msg_q;
    std::int64_t hi_count = 0;
    std::int64_t lo_count = 0;
};

auto part1 = [](module_map modules) -> std::int64_t
{
    messenger m;
    for (auto _ : flux::ints(0, 1000)) { m.run(modules); }

    return m.hi_count * m.lo_count;
};

auto part2 = [](module_map const& modules) -> int64_t
{
    std::vector<int64_t> values;

    // These are specific to my input, sorry about that
    for (std::string name : {"nd", "pc", "vd", "tx"}) {
        auto copy = modules;
        messenger m;
        int counter = 1;
        while (!m.run(copy, name)) {
            ++counter;
        }
        values.push_back(counter);
    }

    return flux::fold(values, [](auto a, auto b) { return std::lcm(a, b); }, 1);
};

constexpr auto& test_data1 =
R"(broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a)";

constexpr auto& test_data2 =
R"(broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output)";

}

int main(int argc, char** argv)
{
    {
        assert(part1(parse_input(test_data1)) == 32000000);
        assert(part2(parse_input(test_data2)) == 11687500);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto input = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}