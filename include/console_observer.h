#pragma once
#include "observer.h"
#include <iostream>

class ConsoleObserver : public Observer {
    public:
        void notify(const std::string& event) override {
            std::cout << event << std::endl;
        }
};