#pragma once
#include <optional>
#include <vector>
#include "DiningTable.h"

class IDiningTableRepository
{
public:
    virtual ~IDiningTableRepository() = default;

    virtual int add(const DiningTable& table) = 0;
    virtual std::optional<DiningTable> findById(int id) = 0;
    virtual std::vector<DiningTable> findAll() = 0;
    virtual bool update(const DiningTable& table) = 0;
    virtual bool remove(int id) = 0;
};