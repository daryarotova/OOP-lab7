#pragma once
#include <string>

class Observer {
    public:
    virtual ~Observer() = default;

    virtual void notify(const std::string& event) = 0;
};