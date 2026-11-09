#pragma once
#include <stdexcept>
#include <string>
#include <sqlite_orm/sqlite_orm.h>
#include "DiningTable.h"
#include "Order.h"

inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;
    return make_storage(
        filename,
        make_table(
            "DiningTable",
            make_column("id", &DiningTable::id, primary_key().autoincrement()),
            make_column("number", &DiningTable::number, not_null()),
            make_column("zone", &DiningTable::zone, not_null()),
            make_column("seats", &DiningTable::seats, not_null())
        ),
        make_table(
            "Orders",
            make_column("id", &Order::id, primary_key().autoincrement()),
            make_column("tableId", &Order::tableId),
            make_column("orderDate", &Order::orderDate, not_null()),
            make_column("dish", &Order::dish, not_null()),
            make_column("status", &Order::status, not_null()),
            make_column("amount", &Order::amount),
            foreign_key(&Order::tableId).references(&DiningTable::id)
        )
    );
}

template <class Storage>
void setupStorage(Storage& storage)
{
    storage.on_open = [](sqlite3* db)
        {
            if (sqlite3_exec(db, "PRAGMA foreign_keys = ON;",
                nullptr, nullptr, nullptr) != SQLITE_OK)
            {
                throw std::runtime_error("Cannot enable foreign keys");
            }
        };
    storage.sync_schema();
}