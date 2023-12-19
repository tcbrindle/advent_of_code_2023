
#include "../aoc.hpp"

#include "../extern/ctre.hpp"

namespace {

using i64 = std::int64_t;

struct rule {
    char cat; // x, m, a or s
    char op; // < or >
    int value;
    std::string dest;
};

struct workflow {
    std::vector<rule> rules;
    std::string fallback;
};

using workflows_map = std::unordered_map<std::string, workflow>;

using part = std::array<int, 4>;

struct range {
    int min, max;
};

using part_range = std::array<range, 4>;

auto parse_rules = [](std::string_view str) -> std::vector<rule>
{
    constexpr auto& rule_regex =
        ctre::match<R"(([xmas])([<>])(\d+):(\w+))">;

    return flux::split_string(str, ',')
            .map([](std::string_view r) -> rule {
                auto [m, cat, op, value, dest] = rule_regex(r);
                assert(m);
                return rule{
                    .cat = cat.view().at(0),
                    .op = op.view().at(0),
                    .value = value.to_number(),
                    .dest = dest.str()
                };
            })
            .to<std::vector>();
};

auto parse_workflows = [](std::string_view str) -> workflows_map
{
    constexpr auto& workflow_regex =
        ctre::match<R"((\w+)\{(.*),(\w+)\})">;

    return flux::split_string(str, '\n')
            .map([](std::string_view line) -> std::pair<std::string, workflow> {
                   auto [m, name, rules, fallback] = workflow_regex(line);
                   assert(m);
                   return std::pair(name.str(),
                                    workflow{.rules = parse_rules(rules),
                                             .fallback = fallback.str()});
             })
            .to<workflows_map>();
};

auto parse_parts = [](std::string_view str) -> std::vector<part>
{
    constexpr auto& regex = ctre::range<R"(\{x=(\d+),m=(\d+),a=(\d+),s=(\d+)\})">;

    return flux::from_crange(regex(str))
            .filter(std::identity{})
            .map([](auto match) -> part {
                auto [_, x, m, a, s] = match;
                return part{x.to_number(), m.to_number(),
                            a.to_number(), s.to_number() };
             })
            .to<std::vector>();
};

auto parse_input = [](std::string_view input)
    -> std::pair<workflows_map, std::vector<part>>
{
    auto blank_line = input.find("\n\n");

    return {parse_workflows(input.substr(0, blank_line)),
            parse_parts(input.substr(blank_line + 2))};
};

auto category_to_dim = [](char c) -> int {
    switch (c) {
    case 'x': return 0;
    case 'm': return 1;
    case 'a': return 2;
    case 's': return 3;
    default: throw std::runtime_error("Unknown category");
    }
};

auto process_flows_recursive(std::string const& name, part const& part,
                             workflows_map const& workflows) -> bool
{
    if (name == "A") {
        return true;
    } else if (name == "R") {
        return false;
    }

    workflow const& w = workflows.at(name);
    for (const rule& rule : w.rules) {
        const int v = part.at(category_to_dim(rule.cat));
        if ((rule.op == '<' && v < rule.value) || (rule.op == '>' && v > rule.value)) {
            return process_flows_recursive(rule.dest, part, workflows);
        }
    }
    return process_flows_recursive(w.fallback, part, workflows);
}

auto part1 = [](workflows_map const& workflows, std::vector<part> const& parts) -> i64
{
    return flux::ref(parts)
            .filter([&](part const& p) {
                return process_flows_recursive("in", p, workflows);
             })
            .map(flux::sum)
            .sum();
};

enum class comp { below, inside, above };

auto compare_range = [](range rng, int value) -> comp
{
    assert(rng.min <= rng.max);
    if (value < rng.min) {
        return comp::below;
    } else if (value >= rng.max) {
        return comp::above;
    } else {
        return comp::inside;
    }
};

// Returns (accepted_rng, rejected_rng) pair
auto split_range(rule const& rule, part_range const& rng)
    -> std::pair<part_range, part_range>
{
    int const dim = category_to_dim(rule.cat);
    comp c = compare_range(rng.at(dim), rule.value);
    constexpr part_range empty = {};

    auto lower = rng;
    auto upper = rng;

    if (rule.op == '<') {
        if (c == comp::below) {
            upper = empty;
        } else if (c == comp::above) {
            lower = empty;
        } else {
            lower.at(dim).max = -1 + rule.value;
            upper.at(dim).min = rule.value;
        }
        return {lower, upper};
    } else {
        if (c == comp::below) {
            lower = empty;
        } else if (c == comp::above) {
            upper = empty;
        } else {
            lower.at(dim).max = rule.value;
            upper.at(dim).min = 1 + rule.value;
        }
        return {upper, lower};
    }
}

auto part2 = [](workflows_map const& workflows) -> i64
{
    part_range initial_range = {{
        {.min = 1, .max = 4000},
        {.min = 1, .max = 4000},
        {.min = 1, .max = 4000},
        {.min = 1, .max = 4000}
    }};

    std::vector<std::pair<std::string, part_range>> stack;
    stack.emplace_back("in", initial_range);

    i64 count = 0;

    while (!stack.empty()) {
        auto [name, rng] = stack.back();
        stack.pop_back();

        if (name == "A") {
            count += flux::map(rng, [](auto r) -> i64 { return 1 + r.max - r.min; }).product();
        } else if (name == "R") {
            continue;
        } else {
            workflow const& w = workflows.at(name);
            for (const rule& rule : w.rules) {
                auto [accepted, rejected] = split_range(rule, rng);
                stack.emplace_back(rule.dest, accepted);
                rng = rejected;
            }
            stack.emplace_back(w.fallback, rng);
        }
    }

    return count;
};

constexpr auto& test_data =
R"(px{a<2006:qkq,m>2090:A,rfg}
pv{a>1716:R,A}
lnx{m>1548:A,A}
rfg{s<537:gd,x>2440:R,A}
qs{s>3448:A,lnx}
qkq{x<1416:A,crn}
crn{x>2662:A,R}
in{s<1351:px,qqz}
qqz{s>2770:qs,m<1801:hdj,R}
gd{a>3333:R,R}
hdj{m>838:A,pv}

{x=787,m=2655,a=1222,s=2876}
{x=1679,m=44,a=2067,s=496}
{x=2036,m=264,a=79,s=2244}
{x=2461,m=1339,a=466,s=291}
{x=2127,m=1623,a=2188,s=1013})";

}

int main(int argc, char** argv)
{
    {
        auto const [workflows, parts] = parse_input(test_data);
        assert(part1(workflows, parts) == 19114);
        assert(part2(workflows) == 167409079868000);
    }

    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const [workflows, parts] = parse_input(aoc::string_from_file(argv[1]));
    fmt::println("Part 1: {}", part1(workflows, parts));
    fmt::println("Part 2: {}", part2(workflows));
}