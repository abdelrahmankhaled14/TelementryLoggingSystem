#include "consolesink.hpp"
#include <iostream>

void consolesink::write(const logmessage& str)
{
    std::cout << str << std::endl;
}
