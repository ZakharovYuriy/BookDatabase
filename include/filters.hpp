#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

[[nodiscard]] auto YearBetween(int from, int to) {
    return [from, to](const Book &book) { return (book.year >= from) && (book.year <= to); };
}

[[nodiscard]] auto RatingAbove(double targetRating) {
    return [targetRating](const Book &book) { return book.rating >= targetRating; };
}

[[nodiscard]] auto GenreIs(Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

template <BookPredicate... UnaryPredicate>
[[nodiscard]] auto all_of(UnaryPredicate... pred) {
    return [pred...](const Book &book) { return (pred(book) && ...); };
}

template <BookPredicate... UnaryPredicate>
[[nodiscard]] auto any_of(UnaryPredicate... pred) {
    return [pred...](const Book &book) { return (pred(book) || ...); };
}

template <BookIterator BookIt, BookPredicate UnaryPredicate>
[[nodiscard]] auto filterBooks(BookIt begin, BookIt end, UnaryPredicate pred) {
    using Ref = std::iter_reference_t<BookIt>;
    using Elem = std::remove_reference_t<Ref>;
    std::vector<std::reference_wrapper<Elem>> result;
    for (auto it = begin; it != end; ++it) {
        if (std::invoke(pred, *it)) {
            if constexpr (std::is_const_v<Elem>)
                result.push_back(std::cref(*it));  // const Book&
            else
                result.push_back(std::ref(*it));  // Book&
        }
    }
    return result;
}

}  // namespace bookdb