#pragma once

#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    // Type aliases
    using value_type = Book;
    using size_type = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;
    using reference = typename BookContainer::reference;
    using const_reference = typename BookContainer::const_reference;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    using author_value_type = typename AuthorContainer::value_type;
    using author_iterator = typename AuthorContainer::iterator;
    using const_author_iterator = typename AuthorContainer::const_iterator;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init) {
        for (const auto &b : init) {
            PushBack(b);
        }
    }

    bool empty() const noexcept { return books_.empty(); }
    size_type size() const noexcept { return books_.size(); }

    iterator begin() noexcept { return books_.begin(); }
    const_iterator begin() const noexcept { return books_.begin(); }
    const_iterator cbegin() const noexcept { return books_.cbegin(); }

    iterator end() noexcept { return books_.end(); }
    const_iterator end() const noexcept { return books_.end(); }
    const_iterator cend() const noexcept { return books_.cend(); }

    const_author_iterator authors_cbegin() const noexcept { return authors_.cbegin(); }
    const_author_iterator authors_cend() const noexcept { return authors_.cend(); }

    // Standard container interface methods
    void Clear() {
        books_.clear();
        authors_.clear();
    }

    void PushBack(const Book &b) {
        auto [authorIt, emplaced] = authors_.emplace(b.author);
        auto &lastInBook = books_.emplace_back(b);
        lastInBook.author = *authorIt;
    }

    void PushBack(Book &&b) {
        auto [authorIt, emplaced] = authors_.emplace(b.author);
        auto &lastInBook = books_.emplace_back(std::move(b));
        lastInBook.author = *authorIt;
    }

    template <typename... Args>
    reference EmplaceBack(Args &&...args) {
        Book &b = books_.emplace_back(std::forward<Args>(args)...);
        auto [authorIt, emplaced] = authors_.emplace(b.author);
        b.author = *authorIt;
        return b;
    }

    // Getters
    const BookContainer &GetBooks() const noexcept { return books_; }
    const AuthorContainer &GetAuthors() const noexcept { return authors_; }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
