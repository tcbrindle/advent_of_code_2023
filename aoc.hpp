
#ifndef AOC_HPP_INCLUDED
#define AOC_HPP_INCLUDED

#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <flux.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

namespace aoc {



// This function is not great, but nor are the alternatives:
//  * std::from_chars - not constexpr, requires contiguous input
//  * std::atoi - same
//  * std::stoi - needs a std::string, throws on failure
//  * istreams - urgh
// So, we're gonna roll our own
// No, I'm not going to try and do this for floating point...
template <std::integral I>
const auto try_parse = [](flux::sequence auto&& f) -> std::optional<I> {

    //constexpr auto is_space = flow::pred::in(' ', '\f', '\n', '\r', '\t', '\v');
    //constexpr auto is_digit = flow::pred::geq('0') && flow::pred::leq('9');
    constexpr auto is_space = [](char c) {
        return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
    };
    constexpr auto is_digit = [](char c) { return c >= '0' && c <= '9'; };

    auto f2 = flux::drop_while(FLUX_FWD(f), is_space);

    I mult = 1;
    std::optional<I> first{};

    // Deal with the first character
    {
        auto cur = f2.first();
        if (f2.is_last(cur)) {
            return std::nullopt;
        }

        char c = f2[cur];

        if (c == '-') {
            mult = -1;
        } else if (c == '+') {
            // pass
        } else if (is_digit(c)) {
            first = c - '0';
        } else {
            return {};
        }
    }

    // Deal with the rest
    auto res = std::move(f2)
                 .drop(1)
                 .take_while(is_digit)
                  .fold([](auto acc, char c) -> std::optional<I> {
                     return 10 * acc.value_or(0) + (c - '0');
                   }, std::move(first));
    if (res) {
        *res *= mult;
    }
    return res;
};

template <std::integral I>
constexpr auto parse = [](flux::sequence auto&& seq) -> I
{
    return try_parse<I>(FLUX_FWD(seq)).value();
};

template <typename T>
constexpr auto vector_from_file = [](char const* path)
{
    std::ifstream file(path);
    return flux::from_istream<T>(file).template to<std::vector<T>>();
};

constexpr auto string_from_file = [](const char* path)
{
    std::ifstream file(path);
    return flux::from_istreambuf(file).template to<std::string>();
};

struct timer {
    using clock = std::chrono::high_resolution_clock;

    template <typename D = std::chrono::microseconds>
    auto elapsed() const -> D {
        return std::chrono::duration_cast<D>(clock::now() - start_);
    }

    void reset() { start_ = clock::now(); }

private:
    typename clock::time_point start_ = clock::now();
};

}

#endif
