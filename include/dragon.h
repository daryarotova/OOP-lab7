#pragma once
#include <string>
#include "npc.h"

class Dragon : public Npc {
    public:
        Dragon(int x, int y, const std::string& name);

        void accept(Visitor& visitor) override;

        void printInfo() const override;

        int getMoveDistance() const override { return 50; }
        int getKillDistance() const override { return 30; }

    private:
        static const std::string kType;
};