#include "TestData.h"
#include "Database.h"
#include <filesystem>
#include <iostream>
#include <vector>

void prepareTestData()
{
    const std::string databaseFile = "restaurant_queries.db";
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    setupStorage(storage);
    storage.open_forever();

    std::vector<DiningTable> tables = {
        {0, 1, "Hall",    4},
        {0, 2, "Hall",    2},
        {0, 3, "Terrace", 6},
        {0, 4, "Terrace", 4},
        {0, 5, "VIP",     8}
    };

    std::vector<int> ids;
    for (const auto& t : tables)
    {
        ids.push_back(storage.insert(t));
    }

    std::vector<Order> orders = {
        {0, ids[0], "2026-09-01", "Pizza and lemonade",  "paid", 540.0},
        {0, ids[0], "2026-09-15", "Steak and wine",      "paid", 1250.0},
        {0, ids[0], "2026-10-10", "Pasta Carbonara",     "paid", 380.0},
        {0, ids[1], "2026-08-10", "Salad and coffee",    "paid", 290.0},
        {0, ids[1], "2026-09-12", "Soup and bread",      "paid", 210.0},
        {0, ids[1], "2026-10-05", "Burger set",          "open", 450.0},
        {0, ids[2], "2026-07-20", "Family dinner",       "paid", 2200.0},
        {0, ids[2], "2026-08-25", "Grilled fish",        "paid", 780.0},
        {0, ids[2], "2026-10-12", "Dessert platter",     "open", 640.0},
        {0, ids[3], "2026-06-18", "Sushi set",           "paid", 950.0},
        {0, ids[3], "2026-08-30", "Pizza Margherita",    "paid", 330.0},
        {0, ids[3], "2026-10-18", "Tea and cake",        "open", 260.0},
        {0, ids[4], "2026-05-14", "Banquet",             "paid", 5200.0},
        {0, ids[4], "2026-07-22", "Lobster dinner",      "paid", 3100.0},
        {0, ids[4], "2026-09-28", "Wine tasting",        "paid", 1800.0}
    };

    for (const auto& o : orders)
    {
        storage.insert(o);
    }

    std::cout << "\nTest database prepared\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';
}