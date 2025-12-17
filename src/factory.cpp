#include <memory>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "../include/factory.h"
#include "../include/dragon.h"
#include "../include/elf.h"
#include "../include/druid.h"

std::unique_ptr<Npc> NpcFactory::createNpc(
    const std::string& type,
    const std::string& name,
    int x,
    int y)
{
    if (type == "Dragon") {
        return std::make_unique<Dragon>(x, y, name);
    } else if (type == "Elf") {
        return std::make_unique<Elf>(x, y, name);
    } else if (type == "Druid") {
        return std::make_unique<Druid>(x, y, name);
    } else {
        throw std::invalid_argument("Unknown NPC type: " + type);
    }
}

std::unique_ptr<Npc> NpcFactory::createFromString(const std::string& line) {
    std::istringstream iss(line);
    std::string type, name;
    int x, y;

    iss >> type >> name >> x >> y;
    
    if (iss.fail()) {
        throw std::runtime_error("Failed to parse line: " + line);
    }

    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::out_of_range("Coordinates out of range (0-500): " + line);
    }

    return createNpc(type, name, x, y);
}