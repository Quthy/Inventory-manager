#ifndef INVENTORY_H
#define INVENTORY_H

#include <map>
#include <string>
#include <vector>
#include "Product.h"

// Stores all products and handles saving/loading them to a CSV file.
class Inventory {
public:
    // Adds a product and gives it the next free ID. Returns the new ID.
    int addProduct(Product product);

    bool removeProduct(int id);

    // Returns a pointer to the product, or nullptr if the ID does not exist.
    Product* find(int id);
    const Product* find(int id) const;

    // Case-insensitive search on product name.
    std::vector<const Product*> searchByName(const std::string& text) const;

    std::vector<const Product*> lowStock() const;

    // Perishable products that expire within 'days' days (including already expired).
    std::vector<const Product*> expiringWithin(int days, const Date& today) const;

    std::vector<const Product*> all() const;

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

    bool empty() const { return products_.empty(); }

private:
    std::map<int, Product> products_; // kept sorted by ID
    int nextId_ = 1;
};

#endif
