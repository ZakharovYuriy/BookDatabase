#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.author < rhs.author; }
};

struct LessByTitle {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.title < rhs.title; }
};

struct LessByYear {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.year < rhs.year; }
};

struct LessByGenre {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.genre < rhs.genre; }
};

struct LessByPopularity {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.rating < rhs.rating; }
};

struct GreaterByRating {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.rating < rhs.rating; }
};

struct LessByRating {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.rating > rhs.rating; }
};

struct GreaterByReadCount {
    bool operator()(const Book &lhs, const Book &rhs) const noexcept { return lhs.read_count > rhs.read_count; }
};

}  // namespace bookdb::comp