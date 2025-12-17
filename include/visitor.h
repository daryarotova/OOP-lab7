#pragma once

class Dragon;
class Elf;
class Druid;

class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visit(Dragon& dragon) = 0;
        virtual void visit(Elf& elf) = 0;
        virtual void visit(Druid& druid) = 0;
};