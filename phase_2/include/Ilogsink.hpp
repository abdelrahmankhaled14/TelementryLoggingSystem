#pragma once

#include <iostream>
#include "logmessage.hpp"
class Ilogsink
{
public:
    Ilogsink() = default;            
    virtual void write(const logmessage& str) = 0;
    virtual ~Ilogsink() = default;   
};