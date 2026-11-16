#include "Cart.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

Cart::Cart(Inventory& inventory) : inventory_(inventory) {}

int Cart::qtyInCart(int productId) const {
    for (const Line& line : items_) {
        if (line.productId == productId) return line.qty;
    }
    return 0;
}

std::string Cart::addItem(int productId, int qty, const Date& today) {
    const Product* p = inventory_.find(productId);
    if (!p) return "No product with that ID.";
    if (qty <= 0) return "Quantity must be at least 1.";
    if (p->isExpired(today)) return p->name + " is expired and cannot be sold.";

    int available = p->quantity - qtyInCart(productId);
    if (qty > available) {
        return "Not enough stock. Only " + std::to_string(available) + " left.";
    }

    // If the product is already in the cart, just increase the quantity.
    for (Line& line : items_) {
        if (line.productId == productId) {
            line.qty += qty;
            return "";
        }
    }
    items_.push_back({productId, p->name, p->price, qty});
    return "";
}

bool Cart::removeItem(int productId) {
    auto it = std::remove_if(items_.begin(), items_.end(),
                             [productId](const Line& l) { return l.productId == productId; });
    if (it == items_.end()) return false;
    items_.erase(it, items_.end());
    return true;
}

double Cart::subtotal() const {
    double total = 0.0;
    for (const Line& line : items_) total += line.unitPrice * line.qty;
    return total;
}

void Cart::printSummary() const {
    if (items_.empty()) {
        std::cout << "  (cart is empty)\n";
        return;
    }
    std::cout << std::fixed << std::setprecision(2);
    for (const Line& line : items_) {
        std::cout << "  " << std::left << std::setw(22) << line.name
                  << std::right << std::setw(4) << line.qty << " x RM"
                  << std::setw(7) << line.unitPrice << " = RM"
                  << std::setw(8) << line.unitPrice * line.qty << '\n';
    }
    std::cout << "  Subtotal: RM" << subtotal() << '\n';
}

void Cart::checkout(double taxRate) {
    double sub = subtotal();
    double tax = sub * taxRate;
    double total = sub + tax;

    // Deduct the sold units from stock.
    for (const Line& line : items_) {
        Product* p = inventory_.find(line.productId);
        if (p) p->quantity -= line.qty;
    }

    std::cout << "\n========== RECEIPT ==========\n";
    std::cout << "Date: " << Date::today().toString() << "\n\n";
    printSummary();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  SST (" << static_cast<int>(taxRate * 100) << "%): RM" << tax << '\n';
    std::cout << "  TOTAL: RM" << total << '\n';
    std::cout << "=============================\n";
    std::cout << "  Thank you for shopping!\n\n";

    items_.clear();
}
