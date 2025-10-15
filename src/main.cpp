#include <algorithm>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

using namespace bookdb;

int main() {
    //
    // Ниже приведён пример работы `BookDatabase`.
    //
    //     - Обратите внимание, что в этой функции реализованы основные возможности, охватывающие как обязательные, так
    //     и опциональные требования,
    //       которые не обязательны к реализации для сдачи работы.
    //     - Не забудьте перед созданием коммита вызвать 'run_clang_format.sh' для форматирования кода
    //

    // Create a book database
    BookDatabase<std::vector<Book>> db;

    // Add some books
    db.EmplaceBack("George Orwell", "1984", 1949, Genre::SciFi, 4., 190);
    db.EmplaceBack("George Orwell", "Animal Farm", 1945, Genre::Fiction, 4.4, 143);
    db.EmplaceBack("F. Scott Fitzgerald", "The Great Gatsby", 1925, Genre::Fiction, 4.5, 120);
    db.EmplaceBack("Harper Lee", "To Kill a Mockingbird", 1960, Genre::Fiction, 4.8, 156);
    db.EmplaceBack("Jane Austen", "Pride and Prejudice", 1813, Genre::Fiction, 4.7, 178);
    db.EmplaceBack("J.D. Salinger", "The Catcher in the Rye", 1951, Genre::Fiction, 4.3, 112);
    db.EmplaceBack("Aldous Huxley", "Brave New World", 1932, Genre::SciFi, 4.5, 98);
    db.EmplaceBack("Charlotte Brontë", "Jane Eyre", 1847, Genre::Fiction, 4.6, 110);
    db.EmplaceBack("J.R.R. Tolkien", "The Hobbit", 1937, Genre::Fiction, 4.9, 203);
    db.EmplaceBack("William Golding", "Lord of the Flies", 1954, Genre::Fiction, 4.2, 89);
    std::print("Books: {}\n\n", db);

    // Sorts
    std::sort(db.begin(), db.end(), comp::LessByAuthor{});
    std::print("Books sorted by author: {}\n\n==================\n", db);

    std::sort(db.begin(), db.end(), comp::LessByPopularity{});
    std::print("Books sorted by popularity: {}\n\n==================\n", db);

    // Author histogram
    auto histogram = buildAuthorHistogramFlat(db);
    std::print("Author histogram: {}", histogram);

    // Ratings
    auto genreRatings = calculateGenreRatings(db.begin(), db.end());
    std::print("\n\nAverage ratings by genres: {}\n", genreRatings);

    auto avrRating = calculateAverageRating(db);
    std::print("Average books rating in library: {}\n", avrRating);

    // Filters
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    std::print("\n\nBooks from the 20th century with rating ≥ 4.5:\n");
    std::for_each(filtered.cbegin(), filtered.cend(), [](const auto &v) { std::print("{}\n", v.get()); });

    // Top 3 books
    auto topBooks = getTopNBy(db, 3, comp::LessByRating{});
    std::print("\n\nTop 3 books by rating:\n");
    std::for_each(topBooks.cbegin(), topBooks.cend(), [](const auto &v) { std::print("{}\n", v.get()); });

    auto orwellBookIt = std::find_if(db.begin(), db.end(), [](const auto &v) { return v.author == "George Orwell"; });
    if (orwellBookIt != db.end()) {
        std::print("\n\nTransparent lookup by authors. Found Orwell's book: {}\n", *orwellBookIt);
    }

    return 0;
}