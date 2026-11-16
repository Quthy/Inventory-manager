cmake_minimum_required(VERSION 3.10)
project(InventoryManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(inventory
    src/main.cpp
    src/Date.cpp
    src/Inventory.cpp
    src/Cart.cpp
)
target_include_directories(inventory PRIVATE include)
