# Supermarket Inventory Manager

A C++17 console application for running the stock room and checkout counter of a small supermarket.

## Features

- **Product management**: add, restock, change price, remove and search products
- **Expiry tracking**: perishable items carry an expiry date; the app warns on startup about anything expired or expiring within 7 days
- **Low-stock alerts**: each product has a reorder level, and a report lists everything at or below it
- **Checkout**: build a cart by product ID, then print a receipt with 6% SST. Stock is deducted automatically, and expired items or quantities above what's in stock are refused
- **Persistence**: inventory is saved to `data/inventory.csv` after every change and loaded on startup
- **Input validation**: every prompt re-asks until the input is valid, so the program never crashes on bad input

## Project structure

```
include/
  Date.h        Calendar date: parsing, comparison, days between dates
  Product.h     Product data and expiry / low-stock checks
  Inventory.h   Product storage, search, reports, CSV save/load
  Cart.h        Shopping cart and checkout
src/
  Date.cpp
  Inventory.cpp
  Cart.cpp
  main.cpp      Menu and user input
data/
  inventory.csv Sample stock data
```

## Build and run

From the project root:

```bash
g++ -std=c++17 -Wall -Iinclude src/*.cpp -o inventory
./inventory          # Windows: inventory.exe
```

Or with CMake:

```bash
cmake -S . -B build
cmake --build build
```

Run the program from the project root so it can find `data/inventory.csv`.

## Design notes

- Products are stored in a `std::map<int, Product>` keyed by ID, so lookups are fast and listings stay sorted.
- `Date` converts dates to a day count, which makes "days until expiry" a simple subtraction.
- `Cart` holds a reference to the `Inventory`, so it can check stock while items are added and deduct stock only when payment is confirmed.

## Possible next steps

- Barcode / SKU field and lookup
- Sales history and daily revenue report
- Supplier records and automatic reorder list
- Unit tests for `Date` and `Inventory`
