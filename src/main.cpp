// Supermarket Inventory Manager
// A console app for a small supermarket: track stock, expiry dates and run checkout.

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "Cart.h"
#include "Inventory.h"

namespace {

const std::string DATA_FILE = "data/inventory.csv";
const double SST_RATE = 0.06;      // Malaysian sales tax used on receipts
const int EXPIRY_WARNING_DAYS = 7; // "expiring soon" window

// ---------- input helpers (keep asking until the input is valid) ----------

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

// Text fields are saved in a CSV file, so commas are not allowed.
std::string readText(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        if (s.empty()) { std::cout << "  Please enter something.\n"; continue; }
        if (s.find(',') != std::string::npos) { std::cout << "  Commas are not allowed.\n"; continue; }
        return s;
    }
}

int readInt(const std::string& prompt, int min, int max) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            size_t used;
            int value = std::stoi(s, &used);
            if (used == s.size() && value >= min && value <= max) return value;
        } catch (...) {}
        std::cout << "  Enter a whole number between " << min << " and " << max << ".\n";
    }
}

double readPrice(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            size_t used;
            double value = std::stod(s, &used);
            if (used == s.size() && value >= 0) return value;
        } catch (...) {}
        std::cout << "  Enter a valid price, e.g. 4.50\n";
    }
}

bool readYesNo(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt + " (y/n): ");
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
    }
}

Date readDate(const std::string& prompt) {
    while (true) {
        Date d;
        if (Date::parse(readLine(prompt), d)) return d;
        std::cout << "  Use the format YYYY-MM-DD, e.g. 2026-12-31\n";
    }
}

// ---------- display ----------

std::string statusOf(const Product& p, const Date& today) {
    if (p.isExpired(today)) return "EXPIRED";
    if (p.perishable && today.daysUntil(p.expiry) <= EXPIRY_WARNING_DAYS) return "EXPIRING";
    if (p.isLowStock()) return "LOW STOCK";
    return "OK";
}

void printTable(const std::vector<const Product*>& products, const Date& today) {
    if (products.empty()) {
        std::cout << "  No products to show.\n";
        return;
    }
    std::cout << std::left
              << std::setw(5) << "ID" << std::setw(22) << "Name" << std::setw(14) << "Category"
              << std::right << std::setw(9) << "Price" << std::setw(7) << "Qty"
              << "  " << std::left << std::setw(12) << "Expiry" << "Status\n";
    std::cout << std::string(80, '-') << '\n';
    std::cout << std::fixed << std::setprecision(2);
    for (const Product* p : products) {
        std::cout << std::left
                  << std::setw(5) << p->id << std::setw(22) << p->name.substr(0, 21)
                  << std::setw(14) << p->category.substr(0, 13)
                  << std::right << std::setw(9) << p->price << std::setw(7) << p->quantity
                  << "  " << std::left << std::setw(12)
                  << (p->perishable ? p->expiry.toString() : "-")
                  << statusOf(*p, today) << '\n';
    }
}

// ---------- menu actions ----------

void addProduct(Inventory& inv) {
    Product p;
    p.name = readText("Name: ");
    p.category = readText("Category: ");
    p.price = readPrice("Price (RM): ");
    p.quantity = readInt("Quantity in stock: ", 0, 1000000);
    p.reorderLevel = readInt("Reorder level (warn at or below): ", 0, 1000000);
    p.perishable = readYesNo("Does it expire?");
    if (p.perishable) p.expiry = readDate("Expiry date (YYYY-MM-DD): ");
    int id = inv.addProduct(p);
    std::cout << "  Added '" << p.name << "' with ID " << id << ".\n";
}

void restockProduct(Inventory& inv) {
    int id = readInt("Product ID: ", 1, 1000000);
    Product* p = inv.find(id);
    if (!p) { std::cout << "  No product with that ID.\n"; return; }
    int amount = readInt("Units received: ", 1, 1000000);
    p->quantity += amount;
    if (p->perishable && readYesNo("New batch has a different expiry date?")) {
        p->expiry = readDate("New expiry date (YYYY-MM-DD): ");
    }
    std::cout << "  " << p->name << " now has " << p->quantity << " units.\n";
}

void editPrice(Inventory& inv) {
    int id = readInt("Product ID: ", 1, 1000000);
    Product* p = inv.find(id);
    if (!p) { std::cout << "  No product with that ID.\n"; return; }
    std::cout << "  Current price: RM" << std::fixed << std::setprecision(2) << p->price << '\n';
    p->price = readPrice("New price (RM): ");
    std::cout << "  Price updated.\n";
}

void removeProduct(Inventory& inv) {
    int id = readInt("Product ID to remove: ", 1, 1000000);
    const Product* p = inv.find(id);
    if (!p) { std::cout << "  No product with that ID.\n"; return; }
    if (readYesNo("Remove '" + p->name + "'?")) {
        inv.removeProduct(id);
        std::cout << "  Removed.\n";
    }
}

void searchProducts(const Inventory& inv, const Date& today) {
    std::string text = readText("Search name: ");
    printTable(inv.searchByName(text), today);
}

void runCheckout(Inventory& inv, const Date& today) {
    Cart cart(inv);
    std::cout << "\n--- Checkout --- (enter product ID 0 to finish)\n";
    while (true) {
        int id = readInt("Product ID: ", 0, 1000000);
        if (id == 0) break;
        int qty = readInt("Quantity: ", 1, 1000000);
        std::string error = cart.addItem(id, qty, today);
        if (!error.empty()) std::cout << "  " << error << '\n';
        cart.printSummary();
    }
    if (cart.empty()) { std::cout << "  Nothing to check out.\n"; return; }
    if (readYesNo("Confirm payment?")) {
        cart.checkout(SST_RATE);
    } else {
        std::cout << "  Checkout cancelled. Stock unchanged.\n";
    }
}

void printMenu() {
    std::cout << "\n===== SUPERMARKET INVENTORY =====\n"
              << " 1. View all products\n"
              << " 2. Add product\n"
              << " 3. Restock product\n"
              << " 4. Change price\n"
              << " 5. Remove product\n"
              << " 6. Search by name\n"
              << " 7. Expiry report\n"
              << " 8. Low stock report\n"
              << " 9. Checkout\n"
              << " 0. Save and exit\n";
}

} // namespace

int main() {
    Inventory inventory;
    if (inventory.loadFromFile(DATA_FILE)) {
        std::cout << "Loaded inventory from " << DATA_FILE << '\n';
    } else {
        std::cout << "No saved data found. Starting with an empty inventory.\n";
    }

    const Date today = Date::today();
    std::cout << "Today is " << today.toString() << '\n';

    // Warn about expired stock as soon as the program starts.
    auto expiring = inventory.expiringWithin(EXPIRY_WARNING_DAYS, today);
    if (!expiring.empty()) {
        std::cout << "\n!! " << expiring.size()
                  << " product(s) expired or expiring within " << EXPIRY_WARNING_DAYS << " days:\n";
        printTable(expiring, today);
    }

    while (true) {
        printMenu();
        int choice = readInt("Choose: ", 0, 9);
        std::cout << '\n';
        switch (choice) {
            case 1: printTable(inventory.all(), today); break;
            case 2: addProduct(inventory); break;
            case 3: restockProduct(inventory); break;
            case 4: editPrice(inventory); break;
            case 5: removeProduct(inventory); break;
            case 6: searchProducts(inventory, today); break;
            case 7: printTable(inventory.expiringWithin(EXPIRY_WARNING_DAYS, today), today); break;
            case 8: printTable(inventory.lowStock(), today); break;
            case 9: runCheckout(inventory, today); break;
            case 0:
                if (inventory.saveToFile(DATA_FILE)) {
                    std::cout << "Saved to " << DATA_FILE << ". Goodbye!\n";
                } else {
                    std::cout << "Could not save to " << DATA_FILE << "!\n";
                }
                return 0;
        }
        // Save after every change so nothing is lost if the window is closed.
        inventory.saveToFile(DATA_FILE);
    }
}
