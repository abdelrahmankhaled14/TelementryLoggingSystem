#pragma once 
#include <iostream>
#include "Ilogsink.hpp"
#include "logmessage.hpp"
#include <fstream>
#include <memory>
#include <sstream>

class filesink : public Ilogsink

{
    private:
    std::unique_ptr<std::ofstream> ptr;
    public:
    filesink(std::string file);
    ~filesink() = default;
    void write (const logmessage& msg) override;
};
