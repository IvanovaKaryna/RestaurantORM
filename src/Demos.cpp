#include "Demos.h"
#include "Database.h"
#include "OrderStore.h"
#include "SQLiteDiningTableRepository.h"
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

namespace
{
    void printTable(const DiningTable& t)
    {
        std::cout << t.id << " | " << t.number << " | "
            << t.zone << " | " << t.seats << '\n';
    }

    void printOrder(const Order& o)
    {
        std::cout << "ID: " << o.id << ", Date: " << o.orderDate
            << ", Dish: " << o.dish << ", Status: " << o.status
            << ", Amount: " << o.amount << '\n';
    }
}

void runRepositoryDemo()
{
    const std::string file = "restaurant.db";
    std::filesystem::remove(file);

    SQLiteDiningTableRepository sqliteRepository(file);
    IDiningTableRepository& repository = sqliteRepository;

    std::cout << "\nREPOSITORY DEMONSTRATION\n";

    std::cout << "\nCREATE\n";
    int id = repository.add(DiningTable{ 0, 10, "Hall", 4 });
    std::cout << "Inserted table ID: " << id << '\n';

    std::cout << "\nREAD\n";
    auto found = repository.findById(id);
    if (found) { printTable(*found); }

    std::cout << "\nUPDATE\n";
    if (found)
    {
        found->seats = 6;
        bool updated = repository.update(*found);
        std::cout << "Updated: " << std::boolalpha << updated << '\n';
        auto after = repository.findById(id);
        if (after) { std::cout << "New seats: " << after->seats << '\n'; }
    }

    std::cout << "\nREAD ALL\n";
    for (const auto& t : repository.findAll()) { printTable(t); }

    std::cout << "\nORDERS FOR TABLE (OrderStore)\n";
    OrderStore orderStore(file);
    orderStore.add(Order{ 0, id, "2026-11-09", "Pizza", "open", 450.0 });
    orderStore.add(Order{ 0, id, "2026-11-09", "Coffee", "open", 90.0 });
    for (const auto& o : orderStore.findByTableId(id)) { printOrder(o); }

    std::cout << "\nDELETE TABLE THAT HAS ORDERS\n";
    try
    {
        repository.remove(id);
        std::cout << "Unexpected removal success\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "Removal rejected: " << ex.what() << '\n';
    }

    std::cout << "\nDELETE TABLE WITHOUT ORDERS\n";
    int freeId = repository.add(DiningTable{ 0, 11, "VIP", 2 });
    bool removed = repository.remove(freeId);
    std::cout << "Removed: " << std::boolalpha << removed << '\n';
    std::cout << "Record exists after deletion: "
        << repository.findById(freeId).has_value() << '\n';
}

void runRelationDemo()
{
    const std::string file = "restaurant_queries.db";
    auto storage = createStorage(file);
    setupStorage(storage);
    OrderStore orderStore(file);

    std::cout << "\nRELATIONSHIP DEMONSTRATION\n";
    std::cout << "\nOrder history of table ID 1:\n";
    for (const auto& o : orderStore.findByTableId(1)) { printOrder(o); }

    try
    {
        Order invalid{ 0, -1, "2026-11-09", "Invalid order", "open", 500.0 };
        storage.insert(invalid);
        std::cout << "Unexpected insertion success\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nForeign key constraint test:\n";
        std::cout << "Insertion rejected: " << ex.what() << '\n';
    }
}

void runQueryDemo()
{
    using namespace sqlite_orm;
    auto storage = createStorage("restaurant_queries.db");
    storage.sync_schema();

    std::cout << "\nORM QUERY DEMONSTRATION\n";

    std::cout << "\n1. Tables in zone Terrace:\n";
    auto terrace = storage.get_all<DiningTable>(
        where(c(&DiningTable::zone) == "Terrace"));
    for (const auto& t : terrace) { printTable(t); }

    std::cout << "\n2. Tables sorted by number (descending):\n";
    auto sorted = storage.get_all<DiningTable>(
        order_by(&DiningTable::number).desc());
    for (const auto& t : sorted) { printTable(t); }

    std::cout << "\n3. Top 3 orders by amount:\n";
    auto top = storage.get_all<Order>(
        order_by(&Order::amount).desc(),
        limit(3));
    for (const auto& o : top)
    {
        std::cout << o.id << " | " << o.dish << " | " << o.amount << '\n';
    }

    std::cout << "\n4. Open orders sorted by amount:\n";
    auto open = storage.get_all<Order>(
        where(c(&Order::status) == "open"),
        order_by(&Order::amount).desc());
    for (const auto& o : open)
    {
        std::cout << o.id << " | " << o.dish << " | " << o.amount << '\n';
    }
}

void runAggregateDemo()
{
    using namespace sqlite_orm;
    auto storage = createStorage("restaurant_queries.db");
    storage.sync_schema();

    std::cout << "\nAGGREGATE QUERY DEMONSTRATION\n";

    auto count = storage.count<Order>();
    std::cout << "\n1. Orders count: " << count << '\n';

    auto total = storage.sum(&Order::amount);
    std::cout << "\n2. Total amount: " << (total ? *total : 0.0) << '\n';

    auto average = storage.avg(&Order::amount);
    std::cout << "\n3. Average amount: " << average << '\n';

    std::cout << "\n4. Total amount by table:\n";
    auto byTable = storage.select(
        columns(&Order::tableId, sum(&Order::amount)),
        group_by(&Order::tableId));
    for (const auto& [tableId, amount] : byTable)
    {
        std::cout << "Table ID: " << tableId
            << " | Total: " << (amount ? *amount : 0.0) << '\n';
    }
}

void runJoinDemo()
{
    using namespace sqlite_orm;
    auto storage = createStorage("restaurant_queries.db");
    storage.sync_schema();

    std::cout << "\nJOIN QUERY DEMONSTRATION\n";

    std::cout << "\n1. Orders with table info:\n";
    auto records = storage.select(
        columns(&DiningTable::number, &DiningTable::zone,
            &Order::orderDate, &Order::dish, &Order::amount),
        inner_join<Order>(on(c(&DiningTable::id) == &Order::tableId)),
        order_by(&Order::id));
    for (const auto& [number, zone, date, dish, amount] : records)
    {
        std::cout << "Table " << number << " | " << zone << " | "
            << date << " | " << dish << " | " << amount << '\n';
    }

    std::cout << "\n2. Orders over 1000:\n";
    auto expensive = storage.select(
        columns(&DiningTable::number, &Order::dish, &Order::amount),
        inner_join<Order>(on(c(&DiningTable::id) == &Order::tableId)),
        where(c(&Order::amount) > 1000.0),
        order_by(&Order::amount).desc());
    for (const auto& [number, dish, amount] : expensive)
    {
        std::cout << "Table " << number << " | " << dish
            << " | " << amount << '\n';
    }
}

void runTransactionDemo()
{
    auto storage = createStorage("restaurant_queries.db");
    setupStorage(storage);

    std::cout << "\nTRANSACTION DEMONSTRATION\n";
    std::cout << "\nBefore transaction:\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';

    bool committed = storage.transaction([&]()
        {
            DiningTable table{ 0, 6, "VIP", 10 };
            int tableId = storage.insert(table);

            Order order{ 0, tableId, "2026-11-09", "Welcome dinner", "open", 1500.0 };
            storage.insert(order);

            std::cout << "\nRecords inserted inside transaction\n";
            std::cout << "Table ID: " << tableId << '\n';
            return true; 
        });

    std::cout << "\nTransaction committed: "
        << std::boolalpha << committed << '\n';
    std::cout << "\nAfter transaction:\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';
}

void runRollbackDemo()
{
    using namespace sqlite_orm;
    auto storage = createStorage("restaurant_queries.db");
    setupStorage(storage);

    std::cout << "\nROLLBACK DEMONSTRATION\n";
    std::cout << "\nBefore transaction:\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';

    try
    {
        storage.transaction([&]()
            {
                DiningTable table{ 0, 7, "Hall", 4 };
                int tableId = storage.insert(table);
                std::cout << "\nTable inserted, ID: " << tableId << '\n';

                Order order{ 0, -1, "2026-11-09", "Welcome dinner", "open", 800.0 };
                storage.insert(order);
                return true;
            });
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nTransaction failed: " << ex.what() << '\n';
        std::cout << "Changes rolled back\n";
    }

    std::cout << "\nAfter transaction:\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';

    auto seven = storage.get_all<DiningTable>(
        where(c(&DiningTable::number) == 7));
    std::cout << "\nTable number 7 exists: "
        << std::boolalpha << !seven.empty() << '\n';
}

void runWithoutTransactionDemo()
{
    const std::string file = "restaurant_no_transaction.db";
    std::filesystem::remove(file);

    auto storage = createStorage(file);
    setupStorage(storage);

    std::cout << "\nWITHOUT TRANSACTION\n";

    try
    {
        DiningTable table{ 0, 7, "Hall", 4 };
        int tableId = storage.insert(table);
        std::cout << "\nTable inserted, ID: " << tableId << '\n';

        Order order{ 0, -1, "2026-11-09", "Welcome dinner", "open", 800.0 };
        storage.insert(order);
    }
    catch (const std::exception& ex)
    {
        std::cout << "\nOperation failed: " << ex.what() << '\n';
    }

    std::cout << "\nAfter operations:\n";
    std::cout << "Tables: " << storage.count<DiningTable>() << '\n';
    std::cout << "Orders: " << storage.count<Order>() << '\n';
}