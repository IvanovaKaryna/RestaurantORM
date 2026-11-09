#pragma once
#include <string>
#include <vector>
#include "Database.h"

class OrderStore
{
public:
    explicit OrderStore(const std::string& databaseFile);

    int add(const Order& order);
    std::vector<Order> findByTableId(int tableId);
    std::vector<Order> findAll();

private:
    using Storage = decltype(createStorage(""));
    Storage storage_;
};