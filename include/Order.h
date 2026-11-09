#pragma once
#include <string>

struct Order
{
    int id = 0;
    int tableId = 0;
    std::string orderDate;
    std::string dish;
    std::string status;
    double amount = 0.0;
};