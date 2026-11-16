#include "Inventory.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

int Inventory::addProduct(Product product) {
    product.id = nextId_++;
    products_[product.id] = product;
    return product.id;
}

bool Inventory::removeProduct(int id) {
    return products_.erase(id) > 0;
}

Product* Inventory::find(int id) {
    auto it = products_.find(id);
    return it == products_.end() ? nullptr : &it->second;
}

const Product* Inventory::find(int id) const {
    auto it = products_.find(id);
    return it == products_.end() ? nullptr : &it->second;
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<const Product*> Inventory::searchByName(const std::string& text) const {
    std::vector<const Product*> results;
    std::string needle = toLower(text);
    for (const auto& entry : products_) {
        if (toLower(entry.second.name).find(needle) != std::string::npos) {
            results.push_back(&entry.second);
        }
    }
    return results;
}

std::vector<const Product*> Inventory::lowStock() const {
    std::vector<const Product*> results;
    for (const auto& entry : products_) {
        if (entry.second.isLowStock()) results.push_back(&entry.second);
    }
    return results;
}

std::vector<const Product*> Inventory::expiringWithin(int days, const Date& today) const {
    std::vector<const Product*> results;
    for (const auto& entry : products_) {
        const Product& p = entry.second;
        if (p.perishable && today.daysUntil(p.expiry) <= days) {
            results.push_back(&p);
        }
    }
    // Soonest expiry first.
    std::sort(results.begin(), results.end(),
              [](const Product* a, const Product* b) { return a->expiry < b->expiry; });
    return results;
}

std::vector<const Product*> Inventory::all() const {
    std::vector<const Product*> results;
    for (const auto& entry : products_) results.push_back(&entry.second);
    return results;
}

// CSV format, one product per line:
// id,name,category,price,quantity,reorderLevel,perishable,expiry
bool Inventory::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    out << "id,name,category,price,quantity,reorderLevel,perishable,expiry\n";
    for (const auto& entry : products_) {
        const Product& p = entry.second;
        out << p.id << ',' << p.name << ',' << p.category << ',' << p.price << ','
            << p.quantity << ',' << p.reorderLevel << ',' << (p.perishable ? 1 : 0) << ','
            << (p.perishable ? p.expiry.toString() : "-") << '\n';
    }
    return true;
}

bool Inventory::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    products_.clear();
    nextId_ = 1;

    std::string line;
    std::getline(in, line); // skip header row

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        while (std::getline(ss, field, ',')) fields.push_back(field);
        if (fields.size() != 8) continue; // skip broken lines

        try {
            Product p;
            p.id = std::stoi(fields[0]);
            p.name = fields[1];
            p.category = fields[2];
            p.price = std::stod(fields[3]);
            p.quantity = std::stoi(fields[4]);
            p.reorderLevel = std::stoi(fields[5]);
            p.perishable = fields[6] == "1";
            if (p.perishable && !Date::parse(fields[7], p.expiry)) continue;

            products_[p.id] = p;
            nextId_ = std::max(nextId_, p.id + 1);
        } catch (...) {
            continue; // skip lines with bad numbers
        }
    }
    return true;
}
