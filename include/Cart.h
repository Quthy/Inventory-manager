#ifndef CART_H
#define CART_H

#include <string>
#include <vector>
#include "Inventory.h"

// A customer's shopping cart during checkout.
class Cart {
public:
    explicit Cart(Inventory& inventory);

    // Adds 'qty' units of a product. Returns an error message, or "" on success.
    std::string addItem(int productId, int qty, const Date& today);

    bool removeItem(int productId);

    double subtotal() const;
    bool empty() const { return items_.empty(); }

    void printSummary() const;

    // Takes the items out of stock and prints a receipt. Clears the cart.
    void checkout(double taxRate);

private:
    struct Line {
        int productId;
        std::string name;
        double unitPrice;
        int qty;
    };

    Inventory& inventory_;
    std::vector<Line> items_;

    int qtyInCart(int productId) const;
};

#endif
