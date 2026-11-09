#include "Demos.h"
#include "Database.h"
#include <sqlite3.h>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace
{
    int insertTableRaw(sqlite3* db, const DiningTable& t)
    {
        const char* sql =
            "INSERT INTO DiningTable (number, zone, seats) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(sqlite3_errmsg(db));
        }

        rc = sqlite3_bind_int(stmt, 1, t.number);
        if (rc == SQLITE_OK)
        {
            rc = sqlite3_bind_text(stmt, 2, t.zone.c_str(), -1, SQLITE_TRANSIENT);
        }
        if (rc == SQLITE_OK)
        {
            rc = sqlite3_bind_int(stmt, 3, t.seats);
        }
        if (rc != SQLITE_OK)
        {
            std::string error = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error(error);
        }

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::string error = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error(error);
        }

        int id = static_cast<int>(sqlite3_last_insert_rowid(db));
        sqlite3_finalize(stmt);
        return id;
    }

    std::optional<DiningTable> findTableRaw(sqlite3* db, int id)
    {
        const char* sql =
            "SELECT id, number, zone, seats FROM DiningTable WHERE id = ?;";
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(sqlite3_errmsg(db));
        }

        rc = sqlite3_bind_int(stmt, 1, id);
        if (rc != SQLITE_OK)
        {
            std::string error = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error(error);
        }

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW)
        {
            DiningTable t;
            t.id = sqlite3_column_int(stmt, 0);
            t.number = sqlite3_column_int(stmt, 1);
            t.zone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            t.seats = sqlite3_column_int(stmt, 3);
            sqlite3_finalize(stmt);
            return t;
        }
        if (rc != SQLITE_DONE)
        {
            std::string error = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw std::runtime_error(error);
        }

        sqlite3_finalize(stmt);
        return std::nullopt;
    }

    void printTable(const DiningTable& t)
    {
        std::cout << "ID: " << t.id << ", Number: " << t.number
            << ", Zone: " << t.zone << ", Seats: " << t.seats << '\n';
    }
}

void runCompareDemo()
{
    const std::string file = "restaurant_compare.db";
    std::filesystem::remove(file);

    std::cout << "\nSQLITE C API\n";
    {
        sqlite3* db = nullptr;
        int rc = sqlite3_open(file.c_str(), &db);
        if (rc != SQLITE_OK)
        {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << '\n';
            sqlite3_close(db);
            throw std::runtime_error("open failed");
        }

        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS DiningTable (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                number INTEGER NOT NULL,
                zone TEXT NOT NULL,
                seats INTEGER NOT NULL
            );
        )";
        char* errorMessage = nullptr;
        rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage);
        if (rc != SQLITE_OK)
        {
            std::string error = errorMessage ? errorMessage : "Unknown error";
            sqlite3_free(errorMessage);
            sqlite3_close(db);
            throw std::runtime_error(error);
        }

        try
        {
            DiningTable t{ 0, 100, "Hall", 4 };
            int id = insertTableRaw(db, t);
            std::cout << "Inserted table ID: " << id << '\n';
            auto found = findTableRaw(db, id);
            if (found) { printTable(*found); }
            else { std::cout << "Table not found\n"; }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Database error: " << ex.what() << '\n';
        }
        sqlite3_close(db);
    }

    std::cout << "\nORM\n";
    {
        auto storage = createStorage(file);
        setupStorage(storage);

        DiningTable t{ 0, 101, "Terrace", 6 };
        int id = storage.insert(t);
        std::cout << "Inserted table ID: " << id << '\n';
        auto found = storage.get_optional<DiningTable>(id);
        if (found) { printTable(*found); }
        else { std::cout << "Table not found\n"; }
    }
}