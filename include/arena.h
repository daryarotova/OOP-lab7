#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <shared_mutex>
#include <queue>
#include <atomic>
#include <thread>
#include <condition_variable>
#include "npc.h"
#include "observer.h"

#define MAX_WIDTH 100
#define MAX_HEIGHT 100

struct BattleTask {
    Npc* attacker;
    Npc* defender;
};

class Arena {
    public:
        Arena(int width = MAX_WIDTH, int height = MAX_HEIGHT);
        ~Arena();

        void addNpc(std::unique_ptr<Npc> npc);
        void createAndAddNpc(const std::string& type, const std::string& name, int x, int y);
        void printAllNpcs() const;

        size_t getNpcCount() const;
        size_t getAliveCount() const;
        std::vector<Npc*> getAliveNpcs() const;

        void addObserver(std::shared_ptr<Observer> observer);
        void removeObserver(std::shared_ptr<Observer> observer);

        void startBattle(double range);
        void saveToFile(const std::string& filename) const;
        void loadFromFile(const std::string& filename);
        void clear();

        void startGame(int durationSeconds = 30);
        void stopGame();
        void generateRandomNpcs(int count);
        void printMap() const;
        void printSurvivors() const;

        std::thread& getMovementThread() { return movement_thread_; }
        std::thread& getBattleThread() { return battle_thread_; }
        std::thread& getPrintThread() { return print_thread_; }

    private:
        int width_;
        int height_;
        std::map<std::string, std::unique_ptr<Npc>> npcs_;

        std::vector<std::shared_ptr<Observer>> observers_;

        mutable std::shared_mutex npcs_mutex_;
        mutable std::mutex observers_mutex_;
        mutable std::mutex cout_mutex_;
        
        std::queue<BattleTask> battle_queue_;
        std::mutex battle_queue_mutex_;
        std::condition_variable battle_cv_;

        std::atomic<bool> running_;

        std::thread movement_thread_;
        std::thread battle_thread_;
        std::thread print_thread_;

        void notifyObservers(const std::string& event);
        void movementThreadFunc();
        void battleThreadFunc();
        void printThreadFunc(int durationSeconds);
        bool isValidPosition(int x, int y) const;
};