#include "SQLiteDiningTableRepository.h"

SQLiteDiningTableRepository::SQLiteDiningTableRepository(
    const std::string& databaseFile)
    : storage_(createStorage(databaseFile))
{
    setupStorage(storage_);
}

int SQLiteDiningTableRepository::add(const DiningTable& table)
{
    return storage_.insert(table);
}

std::optional<DiningTable> SQLiteDiningTableRepository::findById(int id)
{
    return storage_.get_optional<DiningTable>(id);
}

std::vector<DiningTable> SQLiteDiningTableRepository::findAll()
{
    return storage_.get_all<DiningTable>();
}

bool SQLiteDiningTableRepository::update(const DiningTable& table)
{
    auto existing = storage_.get_optional<DiningTable>(table.id);
    if (!existing)
    {
        return false;
    }
    storage_.update(table);
    return true;
}

bool SQLiteDiningTableRepository::remove(int id)
{
    auto existing = storage_.get_optional<DiningTable>(id);
    if (!existing)
    {
        return false;
    }
    storage_.remove<DiningTable>(id);
    return true;
}