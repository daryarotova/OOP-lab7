#pragma once
#include <string>
#include <memory>
#include <mutex>

class Visitor;

class Npc {
    public:
        Npc(int x, int y, const std::string& type, const std::string& name);

        virtual ~Npc() = default;
        int getX() const;
        int getY() const;
        std::string getType() const;
        std::string getName() const;

        void setX(int x);
        void setY(int y);
        void setPosition(int x, int y);

        double distanceTo(const Npc& other) const;

        virtual void accept(Visitor& visitor) = 0;

        virtual void printInfo() const;

        friend std::ostream& operator<<(std::ostream& os, const Npc& npc);

        bool isAlive() const;
        void kill();

        virtual int getMoveDistance() const = 0;
        virtual int getKillDistance() const = 0;

    protected:
        mutable std::mutex mutex_;

    private:
        int x_;
        int y_;
        std::string type_;
        std::string name_;
        bool alive_;
};