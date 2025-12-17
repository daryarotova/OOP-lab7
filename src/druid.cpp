#include "../include/druid.h"
#include "../include/visitor.h"
#include <iostream>

const std::string Druid::kType = "Druid";

Druid::Druid(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

void Druid::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Druid::printInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Druid " << getName() << " at (" << getX() << ", " << getY() << ")" << std::endl;
}