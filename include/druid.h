#pragma once
#include <string>
#include "npc.h"

class Druid : public Npc {
    public:
        Druid(int x, int y, const std::string& name);

        void accept(Visitor& visitor) override;

        void printInfo() const override;

        int getMoveDistance() const override { return 10; }
        int getKillDistance() const override { return 10; }

    private:
        static const std::string kType;
};