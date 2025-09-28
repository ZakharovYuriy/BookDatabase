#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept { return lhs < rhs; }
};

struct TransparentStringEqual {
    using is_transparent = void;

    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept { return lhs == rhs; }
};

struct TransparentStringHash {
    using is_transparent = void;

    std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

}  // namespace bookdb
