#pragma once
#include <memory>
#include <string>
#include "npc.h"

class NpcFactory {
    public:
        static std::unique_ptr<Npc> createNpc(
            const std::string& type,
            const std::string& name,
            int x, 
            int y
        );
        
        static std::unique_ptr<Npc> createFromString(const std::string& line);
};