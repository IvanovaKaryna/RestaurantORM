#pragma once
#include <string>
#include "IDiningTableRepository.h"
#include "Database.h"

class SQLiteDiningTableRepository : public IDiningTableRepository
{
public:
    explicit SQLiteDiningTableRepository(const std::string& databaseFile);

    int add(const DiningTable& table) override;
    std::optional<DiningTable> findById(int id) override;
    std::vector<DiningTable> findAll() override;
    bool update(const DiningTable& table) override;
    bool remove(int id) override;

private:
    using Storage = decltype(createStorage(""));
    Storage storage_;
};