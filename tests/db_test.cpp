#include <book_database.hpp>
#include <gtest/gtest.h>

using namespace bookdb;

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

// Гетерогенный поиск авторов
TEST(BookDatabaseTest, HeterogeneousAuthorLookup) {
    BookDatabase<> db{{"Orwell", "Animal Farm", 1945, Genre::Fiction, 4.8, 1500}};

    EXPECT_TRUE(db.GetAuthors().contains(std::string_view("Orwell")));

    EXPECT_TRUE(db.GetAuthors().contains("Orwell"));

    EXPECT_FALSE(db.GetAuthors().contains("Unknown"));
}
