
#include "YouTalkingToMe.hpp"
#include <iostream>

int main()
{
    try {
        YouTalkingToMe app("config.json");
        app.start();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}