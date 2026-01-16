#pragma once

class safesocket
{
private:
public:
    safesocket();
    ~safesocket();
    safesocket(safesocket &&obj);
    safesocket(const safesocket &obj) = delete;
};
