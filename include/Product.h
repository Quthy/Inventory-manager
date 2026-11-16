#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include "Date.h"

// One product line in the supermarket (e.g. "Milk 1L").
struct Product {
    int id = 0;
    std::string name;
    std::string category;
    double price = 0.0;      // price per unit, in RM
    int quantity = 0;        // units in stock
    int reorderLevel = 5;    // warn when stock falls to this level or below
    bool perishable = false; // only perishable items have an expiry date
    Date expiry;

    bool isExpired(const Date& today) const {
        return perishable && expiry < today;
    }

    bool isLowStock() const {
        return quantity <= reorderLevel;
    }
};

#endif
