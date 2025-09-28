#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

const std::unordered_map<Genre, std::string_view> genresToStr{
    {Genre::Fiction, "Fiction"}, {Genre::Mystery, "Mystery"},     {Genre::NonFiction, "NonFiction"},
    {Genre::SciFi, "SciFi"},     {Genre::Biography, "Biography"}, {Genre::Unknown, "Unknown"},
};

const std::unordered_map<std::string_view, Genre> strToGenres{{"Fiction", Genre::Fiction},
                                                              {"NonFiction", Genre::NonFiction},
                                                              {"SciFi", Genre::SciFi},
                                                              {"Biography", Genre::Biography},
                                                              {"Mystery", Genre::Mystery}};

// Ваш код для constexpr преобразования строк в enum::Genre и наоборот здесь

constexpr Genre GenreFromString(std::string_view genre) {
    return strToGenres.contains(genre) ? strToGenres.at(genre) : Genre::Unknown;
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string_view in_author, const std::string &in_title, int in_year, Genre in_genre,
                   double in_rating, int in_read_count)
        : author(in_author), title(in_title), year(in_year), genre(in_genre), rating(in_rating),
          read_count(in_read_count) {}

    constexpr Book(std::string_view in_author, const std::string &in_title, int in_year, std::string_view in_genre,
                   double in_rating, int in_read_count)
        : Book(in_author, in_title, in_year, GenreFromString(in_genre), in_rating, in_read_count) {}
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre genre, FormatContext &formatContext) const {
        using namespace bookdb;
        if (!genresToStr.contains(genre))
            throw logic_error{"Unsupported bookdb::Genre"};

        return format_to(formatContext.out(), "{}", genresToStr.at(genre));
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book book, FormatContext &formatContext) const {
        return format_to(formatContext.out(), "{}", book.title);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std
