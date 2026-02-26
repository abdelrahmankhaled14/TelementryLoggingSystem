#pragma once
#include <iostream>
#include "Ilogsink.hpp"
#include "logmessage.hpp"
class consolesink : public Ilogsink
{
public:
    consolesink() = default;
    ~consolesink() = default;
    void write(const logmessage& str) override;
};