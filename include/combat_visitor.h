#pragma once
#include "visitor.h"
#include "npc.h"

class CombatVisitor : public Visitor {
    public:
        // Метод: может ли атакующий убить защищающегося?
        bool canKill(Npc* attacker, Npc* defender);

        void visit(Dragon&) override {}
        void visit(Elf&) override {}
        void visit(Druid&) override {}
    
    private:
        // Дракон нападает на эльфов
        bool dragonVs(const std::string& defenderType);
        // Эльф нападает на друидов
        bool elfVs(const std::string& defenderType);
        // Друид нападает на драконов
        bool druidVs(const std::string& defenderType);
};