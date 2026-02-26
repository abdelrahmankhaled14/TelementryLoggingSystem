#include "filesink.hpp"
void filesink::write(const logmessage& msg)
{
    std::ostringstream oss;
    oss << msg << '\n';   

    std::string formatted = oss.str();
    ptr->write(formatted.data(), formatted.size());
}


filesink::filesink(std::string file)
{
    ptr = std::make_unique<std::ofstream>(file);
}