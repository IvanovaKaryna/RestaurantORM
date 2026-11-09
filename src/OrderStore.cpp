#include "OrderStore.h"

OrderStore::OrderStore(const std::string& databaseFile)
    : storage_(createStorage(databaseFile))
{
    setupStorage(storage_);
}

int OrderStore::add(const Order& order)
{
    return storage_.insert(order);
}

std::vector<Order> OrderStore::findByTableId(int tableId)
{
    using namespace sqlite_orm;
    return storage_.get_all<Order>(where(c(&Order::tableId) == tableId));
}

std::vector<Order> OrderStore::findAll()
{
    return storage_.get_all<Order>();
}