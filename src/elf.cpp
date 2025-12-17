#include "../include/elf.h"
#include "../include/visitor.h"
#include <iostream>

const std::string Elf::kType = "Elf";

Elf::Elf(int x, int y, const std::string& name)
    : Npc(x, y, kType, name) {}

void Elf::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Elf::printInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "Elf " << getName() << " at (" << getX() << ", " << getY() << ")" << std::endl;
}