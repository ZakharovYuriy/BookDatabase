#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace bookdb;
using namespace bookdb::comp;
using namespace std::literals;

// Конструктор от initializer_list
TEST(BookDatabaseTest, InitializerListConstructor) {
    BookDatabase<> db{{"Orwell", "1984", 1949, Genre::Fiction, 4.9, 1000},
                      {"Tolkien", "The Hobbit", 1937, Genre::Fiction, 4.8, 2000}};

    EXPECT_EQ(db.size(), 2);
    EXPECT_TRUE(db.GetAuthors().contains(std::string_view("Orwell")));
    EXPECT_TRUE(db.GetAuthors().contains(std::string_view("Tolkien")));
}

// PushBack и EmplaceBack
TEST(BookDatabaseTest, PushBackAndEmplaceBack) {
    BookDatabase<> db;

    Book b{"Rowling", "Harry Potter", 1997, Genre::Fiction, 4.7, 5000};
    db.PushBack(b);

    EXPECT_EQ(db.size(), 1);
    EXPECT_TRUE(db.GetAuthors().contains("Rowling"));

    db.EmplaceBack("Asimov", "Foundation", 1951, Genre::SciFi, 4.6, 3000);
    EXPECT_EQ(db.size(), 2);
    EXPECT_TRUE(db.GetAuthors().contains(std::string_view("Asimov")));
}

// Heterogeneous author lookup
TEST(BookDatabaseTest, HeterogeneousAuthorLookup) {
    BookDatabase<> db{{"Orwell", "Animal Farm", 1945, Genre::Fiction, 4.8, 1500}};

    EXPECT_TRUE(db.GetAuthors().contains(std::string_view("Orwell")));

    EXPECT_TRUE(db.GetAuthors().contains("Orwell"));

    EXPECT_FALSE(db.GetAuthors().contains("Unknown"));
}

struct BookDbFixture : ::testing::Test {
    BookDatabase<std::vector<Book>> db;

    void SetUp() override {
        db.EmplaceBack("George Orwell", "1984", 1949, Genre::SciFi, 4.0, 190);
        db.EmplaceBack("George Orwell", "Animal Farm", 1945, Genre::Fiction, 4.4, 143);
        db.EmplaceBack("F. Scott Fitzgerald", "The Great Gatsby", 1925, Genre::Fiction, 4.5, 120);
        db.EmplaceBack("Harper Lee", "To Kill a Mockingbird", 1960, Genre::Fiction, 4.8, 156);
        db.EmplaceBack("Jane Austen", "Pride and Prejudice", 1813, Genre::Fiction, 4.7, 178);
        db.EmplaceBack("J.D. Salinger", "The Catcher in the Rye", 1951, Genre::Fiction, 4.3, 112);
        db.EmplaceBack("Aldous Huxley", "Brave New World", 1932, Genre::SciFi, 4.5, 98);
        db.EmplaceBack("Charlotte Brontë", "Jane Eyre", 1847, Genre::Fiction, 4.6, 110);
        db.EmplaceBack("J.R.R. Tolkien", "The Hobbit", 1937, Genre::Fiction, 4.9, 203);
        db.EmplaceBack("William Golding", "Lord of the Flies", 1954, Genre::Fiction, 4.2, 89);
    }
};

TEST(BuildAuthorHistogramFlat, EmptyDatabase) {
    BookDatabase<std::vector<Book>> empty;
    auto hist = buildAuthorHistogramFlat(empty);
    EXPECT_TRUE(hist.empty());
}

TEST_F(BookDbFixture, BuildAuthorHistogramFlat_CountsAndKeys) {
    auto hist = buildAuthorHistogramFlat(db);

    // Total unique authors: 9 (Orwell has 2 books)
    EXPECT_EQ(hist.size(), 9u);

    // Orwell — 2 books, all others have 1 each
    EXPECT_EQ(hist["George Orwell"sv], 2);
    EXPECT_EQ(hist["F. Scott Fitzgerald"sv], 1);
    EXPECT_EQ(hist["Harper Lee"sv], 1);
    EXPECT_EQ(hist["Jane Austen"sv], 1);
    EXPECT_EQ(hist["J.D. Salinger"sv], 1);
    EXPECT_EQ(hist["Aldous Huxley"sv], 1);
    EXPECT_EQ(hist["Charlotte Brontë"sv], 1);
    EXPECT_EQ(hist["J.R.R. Tolkien"sv], 1);
    EXPECT_EQ(hist["William Golding"sv], 1);
}

// calculateGenreRatings
TEST(CalculateGenreRatings, EmptyRange) {
    std::vector<Book> books;
    auto res = calculateGenreRatings(books.begin(), books.end());
    EXPECT_TRUE(res.empty());
}

TEST_F(BookDbFixture, CalculateGenreRatings_AveragesPerGenre) {
    auto res = calculateGenreRatings(db.begin(), db.end());
    ASSERT_EQ(res.size(), 2u);  // Fiction, SciFi

    // Expected values:
    // SciFi: (4.0 + 4.5) / 2 = 4.25
    // Fiction: (4.4 + 4.5 + 4.8 + 4.7 + 4.3 + 4.6 + 4.9 + 4.2) / 8 = 4.55
    constexpr double eps = 1e-9;
    EXPECT_NEAR(res[Genre::SciFi], 4.25, eps);
    EXPECT_NEAR(res[Genre::Fiction], 4.55, eps);
}

// calculateAverageRating
TEST(CalculateAverageRating, EmptyDatabaseReturnsNaNOrZeroHandled) {
    BookDatabase<std::vector<Book>> empty;
    // If the function doesn’t handle an empty DB internally, division by zero may occur.
    // Then it should either return 0.0, throw an exception,
    (void)calculateAverageRating(empty);
}

TEST_F(BookDbFixture, CalculateAverageRating_WholeLibraryAverage) {
    // Expected average rating across all books:
    // Sum: 4.0 + 4.4 + 4.5 + 4.8 + 4.7 + 4.3 + 4.5 + 4.6 + 4.9 + 4.2 = 44.9
    // Average: 44.9 / 10 = 4.49
    constexpr double eps = 1e-9;
    double expected = 4.49;
    double got = calculateAverageRating(db);
    EXPECT_NEAR(got, expected, eps);
}

// filterBooks + predicates
TEST(FilterBooks, EmptyRange) {
    std::vector<Book> empty;
    auto filtered = filterBooks(empty.begin(), empty.end(), [](const Book &) { return true; });
    EXPECT_TRUE(filtered.empty());
}

TEST_F(BookDbFixture, FilterBooks_ComposePredicates_AllOf) {
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));

    // Matching books: 4.8 (1960), 4.7 (1813 — OUT of range, so excluded),
    // 4.6 (1847 — no), 4.9 (1937 — yes), 4.5 (1932 — SciFi, rating == 4.5 and 1932 in range — yes),
    // 4.5 (1925 — in range — yes).
    // In range 1900..1999 and rating >= 4.5: 1960 (4.8), 1937 (4.9), 1932 (4.5), 1925 (4.5).
    std::set<std::string> titles;
    for (const auto &r : filtered)
        titles.insert(r.get().title);

    EXPECT_TRUE(titles.contains("To Kill a Mockingbird"s));
    EXPECT_TRUE(titles.contains("The Hobbit"s));
    EXPECT_TRUE(titles.contains("Brave New World"s));
    EXPECT_TRUE(titles.contains("The Great Gatsby"s));
    EXPECT_EQ(titles.size(), 4u);
}

TEST_F(BookDbFixture, FilterBooks_ComposePredicates_AnyOf) {
    auto filtered = filterBooks(db.begin(), db.end(), any_of(GenreIs(Genre::SciFi), RatingAbove(4.8)));

    // SciFi: "1984", "Brave New World"
    // Rating > 4.8: "The Hobbit" (4.9)
    std::set<std::string> titles;
    for (const auto &r : filtered)
        titles.insert(r.get().title);

    EXPECT_TRUE(titles.contains("1984"s));
    EXPECT_TRUE(titles.contains("Brave New World"s));
    EXPECT_TRUE(titles.contains("The Hobbit"s));
    EXPECT_EQ(titles.size(), 4u);
}

TEST_F(BookDbFixture, FilterBooks_ConstIteratorsReturnConstRefs) {
    const auto &cdb = db;
    auto filtered = filterBooks(cdb.cbegin(), cdb.cend(), RatingAbove(4.8));

    // Should select books with rating >= 4.8
    // In test data these are "The Hobbit" (4.9) and "To Kill a Mockingbird" (4.8)
    ASSERT_FALSE(filtered.empty());

    // Verify that filterBooks with const iterators returns references to const Book
    using Ref0 = decltype(filtered.front());
    static_assert(std::is_same_v<std::remove_reference_t<Ref0>, std::reference_wrapper<const Book>>,
                  "filterBooks on const iterators must return reference_wrapper<const Book>");

    std::set<std::string> titles;
    for (const auto &ref : filtered)
        titles.insert(ref.get().title);

    EXPECT_TRUE(titles.contains("The Hobbit"s));
    EXPECT_TRUE(titles.contains("To Kill a Mockingbird"s));
    EXPECT_EQ(titles.size(), 2u);
}

// getTopNBy
TEST(GetTopNBy, EmptyDatabase) {
    BookDatabase<std::vector<Book>> empty;
    auto top = getTopNBy(empty, 3, comp::LessByRating{});
    EXPECT_TRUE(top.empty());
}

TEST_F(BookDbFixture, GetTopNBy_PosLEZero) {
    auto top0 = getTopNBy(db, 0, comp::LessByRating{});
    EXPECT_TRUE(top0.empty());
    auto topNeg = getTopNBy(db, -2, comp::LessByRating{});
    EXPECT_TRUE(topNeg.empty());
}

TEST_F(BookDbFixture, GetTopNBy_Top3ByRating_UnorderedCheck) {
    auto top = getTopNBy(db, 3, comp::LessByRating{});
    std::set<std::string> titles;
    for (const auto &r : top)
        titles.insert(r.get().title);

    // Expected top 3: 4.9, 4.8, 4.7
    EXPECT_TRUE(titles.contains("The Hobbit"s));             // 4.9
    EXPECT_TRUE(titles.contains("To Kill a Mockingbird"s));  // 4.8
    EXPECT_TRUE(titles.contains("Pride and Prejudice"s));    // 4.7
    EXPECT_EQ(titles.size(), 3u);
}

// sampleRandomBooks
TEST(SampleRandomBooks, EmptyDatabaseYieldsEmpty) {
    BookDatabase<std::vector<Book>> empty;
    auto sampled = sampleRandomBooks(empty, 5);
    EXPECT_TRUE(sampled.empty());
}

TEST_F(BookDbFixture, SampleRandomBooks_CountZero) {
    auto sampled = sampleRandomBooks(db, 0);
    EXPECT_TRUE(sampled.empty());
}

TEST_F(BookDbFixture, SampleRandomBooks_CountGreaterThanSize) {
    auto sampled = sampleRandomBooks(db, db.size() + 10);
    // Must select exactly size() unique elements
    EXPECT_EQ(sampled.size(), db.size());

    // Verify that elements are truly unique and belong to the container
    std::set<const Book *> ptrs;
    for (const auto &r : sampled)
        ptrs.insert(&r.get());
    EXPECT_EQ(ptrs.size(), sampled.size());
}

TEST_F(BookDbFixture, SampleRandomBooks_TypicalCase_UniqueAndFromContainer) {
    constexpr std::size_t k = 3;
    auto sampled = sampleRandomBooks(db, k);

    EXPECT_EQ(sampled.size(), k);

    std::set<const Book *> ptrs;
    for (const auto &r : sampled) {
        const Book *p = &r.get();
        ptrs.insert(p);

        // Extra check: pointer must match some element inside the database
        bool found_same_address = false;
        for (auto it = db.begin(); it != db.end(); ++it) {
            if (&(*it) == p) {
                found_same_address = true;
                break;
            }
        }
        EXPECT_TRUE(found_same_address);
    }
    EXPECT_EQ(ptrs.size(), k);  // unique
}