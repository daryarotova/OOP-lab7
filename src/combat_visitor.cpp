#include "../include/combat_visitor.h"

bool CombatVisitor::canKill(Npc* attacker, Npc* defender) {
    if (attacker->getType() == "Dragon") {
        return dragonVs(defender->getType());
    } else if (attacker->getType() == "Elf") {
        return elfVs(defender->getType());
    } else if (attacker->getType() == "Druid") {
        return druidVs(defender->getType());
    }
    return false;
}

bool CombatVisitor::dragonVs(const std::string& defenderType) {
    return (defenderType == "Elf");
}

bool CombatVisitor::elfVs(const std::string& defenderType) {
    return (defenderType == "Druid");
}

bool CombatVisitor::druidVs(const std::string& defenderType) {
    return (defenderType == "Dragon");
}