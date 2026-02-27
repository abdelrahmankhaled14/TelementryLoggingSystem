// main.cpp
#include "YourApplicationName.hpp"
#include <iostream>

int main()
{
    try {
        YourApplicationName app("config.json");
        app.start();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}