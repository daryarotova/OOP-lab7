#include "../include/dragon.h"
#include "../include/visitor.h"
#include <iostream>
#include <random>

const std::string Dragon::kType = "Dragon";

Dragon::Dragon(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

void Dragon::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Dragon::printInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Dragon " << getName() << " at (" << getX() << ", " << getY() << ")" << std::endl;
}