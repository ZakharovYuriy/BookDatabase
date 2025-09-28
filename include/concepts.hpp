#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookIterator = std::input_iterator<T> && std::same_as<std::remove_cvref_t<decltype(*std::declval<T>())>, Book>;

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I>;

template <typename T>
concept BookContainerLike = requires(T t) {
    { std::begin(t) } -> BookIterator;
    { std::end(t) } -> BookSentinel<decltype(std::begin(t))>;
} && std::same_as<std::remove_cvref_t<decltype(*std::begin(std::declval<T &>()))>, Book>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book &>;

template <typename C>
concept BookComparator = std::predicate<C, const Book &, const Book &>;

}  // namespace bookdb