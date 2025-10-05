#pragma once

#include <algorithm>
#include <flat_map>
#include <iterator>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"
#include "comparators.hpp"

#include <print>

namespace std {
template <typename Comparator>
struct formatter<std::flat_map<std::string_view, int, Comparator>, char> {
    constexpr auto parse(std::format_parse_context &fc) { return fc.begin(); }

    template <typename FormatContext>
    auto format(const std::flat_map<std::string_view, int, Comparator> &map, FormatContext &fc) const {
        format_to(fc.out(), "Elements ({})\n", map.size());
        for (const auto &[key, val] : map) {
            format_to(fc.out(), "- {}: {}\n", key, val);
        }
        return fc.out();
    }
};

template <>
struct formatter<std::flat_map<bookdb::Genre, double>, char> {
    constexpr auto parse(std::format_parse_context &fc) { return fc.begin(); }

    template <typename FormatContext>
    auto format(const std::flat_map<bookdb::Genre, double> &map, FormatContext &fc) const {
        format_to(fc.out(), "Elements ({})\n", map.size());
        for (const auto &[key, val] : map) {
            format_to(fc.out(), "- {}: {}\n", key, val);
        }
        return fc.out();
    }
};
}  // namespace std

namespace bookdb {
template <BookContainerLike T, typename Comparator = TransparentStringLess>
[[nodiscard]] auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string_view, int, Comparator> authors_map;
    std::for_each(cont.begin(), cont.end(),
                  [&authors_map](const auto &book) { ++authors_map.try_emplace(book.author, 0).first->second; });
    return authors_map;
}

template <BookIterator BookIt>
[[nodiscard]] auto calculateGenreRatings(BookIt begin, BookIt end) {
    std::flat_map<Genre, std::pair<double, int>> summAndNumberRatingByGenre;
    std::flat_map<Genre, double> ratingByGenre;
    std::for_each(begin, end, [&summAndNumberRatingByGenre, &ratingByGenre](const auto &book) {
        auto key = book.genre;
        auto &[sumRating, number] = summAndNumberRatingByGenre[key];
        ++number;
        sumRating += book.rating;
        ratingByGenre[key] = sumRating / number;
    });
    return ratingByGenre;
}

template <BookContainerLike T>
[[nodiscard]] double calculateAverageRating(const BookDatabase<T> &cont) {
    const auto &ratings = calculateGenreRatings(cont.begin(), cont.end());
    double sumRating = 0;
    for (const auto &[genre, rating] : ratings) {
        sumRating += rating;
    };
    return sumRating / cont.size();
}

template <BookContainerLike T, BookComparator Comporator>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> getTopNBy(BookDatabase<T> &cont, int pos,
                                                                        Comporator comparator = comp::LessByRating{}) {
    std::vector<std::reference_wrapper<const Book>> result;
    if (pos <= 0 || cont.empty())
        return result;
    result.reserve(pos);
    auto nth = cont.begin();
    std::advance(nth, pos);
    std::nth_element(cont.begin(), nth, cont.end(), comparator);

    std::transform(cont.begin(), nth, std::back_inserter(result),
                   [](const Book &book) -> std::reference_wrapper<const Book> { return std::cref(book); });

    return result;
}

template <BookContainerLike T, BookComparator Comporator>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(BookDatabase<T> &cont,
                                                                                std::size_t count = 3) {
    std::vector<std::reference_wrapper<const Book>> result;
    if (cont.empty() || count == 0)
        return result;

    int k = std::min<std::size_t>(count, cont.size());

    std::vector<std::reference_wrapper<const Book>> allBooks;
    allBooks.reserve(cont.size());
    std::transform(cont.begin(), cont.end(), std::back_inserter(allBooks), [](const Book &b) { return std::cref(b); });

    std::random_device rd;
    std::mt19937 gen(rd());

    result.reserve(k);
    std::sample(allBooks.begin(), allBooks.end(), std::back_inserter(result), k, gen);

    return result;
}

}  // namespace bookdb
