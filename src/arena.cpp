#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/combat_visitor.h"

int rollDice() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dice(1, 6);
    return dice(gen);
}

Arena::Arena(int width, int height) 
    : width_(width), height_(height), running_(false) {
    if (width > MAX_WIDTH || height > MAX_HEIGHT) {
        throw std::out_of_range("Arena size exceeds maximum limits.");
    }
}

Arena::~Arena() {
    stopGame();
}

void Arena::addNpc(std::unique_ptr<Npc> npc) {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex_);
    const std::string name = npc->getName();

    if (npc->getX() < 0 || npc->getX() > width_ ||
        npc->getY() < 0 || npc->getY() > height_) {
        throw std::out_of_range("NPC position is out of arena bounds.");
    }

    if (npcs_.find(name) != npcs_.end()) {
        throw std::invalid_argument("NPC with this name already exists.");
    }
    npcs_[name] = std::move(npc);
}

void Arena::createAndAddNpc(const std::string& type, 
                            const std::string& name, 
                            int x, int y) {
    auto npc = NpcFactory::createNpc(type, name, x, y);
    addNpc(std::move(npc));
}

void Arena::printAllNpcs() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    for (const auto& pair : npcs_) {
        std::cout << *(pair.second) << std::endl;
    }
}

size_t Arena::getNpcCount() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    return npcs_.size();
}

size_t Arena::getAliveCount() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    size_t count = 0;
    for (const auto& [name, npc] : npcs_) {
        if (npc->isAlive()) {
            count++;
        }
    }
    return count;
}

void Arena::saveToFile(const std::string& filename) const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    for (const auto& [name, npc] : npcs_) {
        file << npc->getType() << " "
             << npc->getName() << " "
             << npc->getX() << " "
             << npc->getY() << std::endl;
    }
}

void Arena::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto npc = NpcFactory::createFromString(line);
        addNpc(std::move(npc));
    }
}

void Arena::clear() {
    std::unique_lock<std::shared_mutex> lock(npcs_mutex_);
    npcs_.clear();
}

void Arena::addObserver(std::shared_ptr<Observer> observer) {
    std::lock_guard<std::mutex> lock(observers_mutex_);
    observers_.push_back(observer);
}

void Arena::removeObserver(std::shared_ptr<Observer> observer) {
    std::lock_guard<std::mutex> lock(observers_mutex_);
    auto it = std::find(observers_.begin(), observers_.end(), observer);
    if (it != observers_.end()) {
        observers_.erase(it);
    }
}

void Arena::notifyObservers(const std::string& event) {
    std::lock_guard<std::mutex> lock(observers_mutex_);
    for (auto& observer : observers_) {
        observer->notify(event);
    }
}

void Arena::startBattle(double range) {
    CombatVisitor visitor;
    std::vector<std::string> toRemove;

    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    for (auto& [name1, npc1] : npcs_) {
        for (auto& [name2, npc2] : npcs_) {
            if (name1 == name2) continue;
            if (name1 > name2) continue;

            if (npc1->distanceTo(*npc2) > range) continue;
            
            bool npc1KillsNpc2 = visitor.canKill(npc1.get(), npc2.get());
            bool npc2KillsNpc1 = visitor.canKill(npc2.get(), npc1.get());
            
            if (npc1KillsNpc2 && npc2KillsNpc1) {
                std::string event = npc1->getName() + " (" + npc1->getType() + 
                                   ") and " + npc2->getName() + " (" + npc2->getType() + 
                                   ") killed each other";
                notifyObservers(event);
                toRemove.push_back(name1);
                toRemove.push_back(name2);
            } else if (npc1KillsNpc2) {
                std::string event = npc1->getName() + " (" + npc1->getType() + 
                                   ") killed " + npc2->getName() + " (" + npc2->getType() + ")";
                notifyObservers(event);
                toRemove.push_back(name2);
            } else if (npc2KillsNpc1) {
                std::string event = npc2->getName() + " (" + npc2->getType() + 
                                   ") killed " + npc1->getName() + " (" + npc1->getType() + ")";
                notifyObservers(event);
                toRemove.push_back(name1);
            }
        }
    }
    
    lock.unlock();
    
    std::sort(toRemove.begin(), toRemove.end());
    toRemove.erase(std::unique(toRemove.begin(), toRemove.end()), toRemove.end());
    
    std::unique_lock<std::shared_mutex> write_lock(npcs_mutex_);
    for (const auto& name : toRemove) {
        npcs_.erase(name);
    }
}

// методы для многопоточности
void Arena::generateRandomNpcs(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> x_dist(0, width_);
    std::uniform_int_distribution<> y_dist(0, height_);
    std::uniform_int_distribution<> type_dist(0, 2);

    const std::vector<std::string> types = {"Dragon", "Elf", "Druid"};

    std::map<std::string, int> type_counts = {{"Dragon", 0}, {"Elf", 0}, {"Druid", 0}};

    for (int i = 0; i < count; ++i) {
        std::string type = types[type_dist(gen)];
        type_counts[type]++;
        std::string name = type + "_" + std::to_string(i);
        int x = x_dist(gen);
        int y = y_dist(gen);

        try {
            createAndAddNpc(type, name, x, y);
        } catch (const std::exception& e) {
            // если имя уже есть, попробуем иное
            for (int j = 0; j < 100; ++j) {
                name = type + "_" + std::to_string(i) + "_" + std::to_string(j);
                try {
                    createAndAddNpc(type, name, x, y);
                    type_counts[type]++;
                    break;
                } catch (...) {}
            }
        }
    }

    // вывод статистики
    std::cout << "\n=== NPC Generation Statistics ===" << std::endl;
    std::cout << "Dragons: " << type_counts["Dragon"] << std::endl;
    std::cout << "Elves: " << type_counts["Elf"] << std::endl;
    std::cout << "Druids: " << type_counts["Druid"] << std::endl;
    std::cout << "Total: " << count << std::endl;
    std::cout << "================================\n" << std::endl;
}

void Arena::printMap() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    std::lock_guard<std::mutex> cout_lock(cout_mutex_);

    std::vector<std::vector<char>> map(height_ + 1, std::vector<char>(width_ + 1, '.'));

    for (const auto& [name, npc] : npcs_) {
        if (npc->isAlive()) {
            int x = npc->getX();
            int y = npc->getY();
            if (x >= 0 && x <= width_ && y >= 0 && y <= height_) {
                char symbol = '?';
                if (npc->getType() == "Dragon") symbol = 'D';
                else if (npc->getType() == "Elf") symbol = 'E';
                else if (npc->getType() == "Druid") symbol = 'R';
                map[y][x] = symbol;
            }
        }
    }

    std::cout << "\n========== MAP ==========" << std::endl;
        
    for (int y = height_; y >= 0; --y) {
        std::cout << std::setw(3) << y << " | ";
        for (int x = 0; x <= width_; ++x) {
            std::cout << map[y][x];
        }
        std::cout << std::endl;
    }

    std::cout << "    +";
    for (int x = 0; x <= width_; ++x) std::cout << "-";
    std::cout << "+" << std::endl;

    std::cout << "      ";
    for (int x = 0; x <= width_; x += 10) {
        if (x == 0) {
            std::cout << "0";
        } else {
            std::cout << std::setw(9) << x;
        }
    }
    std::cout << std::endl;

    std::cout << "\nAlive: " << getAliveCount() << " / " << npcs_.size() << std::endl;
    std::cout << "========================\n" << std::endl;
}

void Arena::printSurvivors() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    std::lock_guard<std::mutex> cout_lock(cout_mutex_);

    std::cout << "\n===== SURVIVORS =====" << std::endl;
    int count = 0;
    for (const auto& [name, npc] : npcs_) {
        if (npc->isAlive()) {
            std::cout << *npc << std::endl;
            count++;
        }
    }
    std::cout << "Total survivors: " << count << std::endl;
    std::cout << "=====================\n" << std::endl;
}

std::vector<Npc*> Arena::getAliveNpcs() const {
    std::shared_lock<std::shared_mutex> lock(npcs_mutex_);
    std::vector<Npc*> alive;
    for (const auto& [name, npc] : npcs_) {
        if (npc->isAlive()) {
            alive.push_back(npc.get());
        }
    }
    return alive;
}

bool Arena::isValidPosition(int x, int y) const {
    return x >= 0 && x <= width_ && y >= 0 && y <= height_;
}

// ф-ции для потоков
void Arena::movementThreadFunc() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dir_dist(-1, 1);

    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto alive_npcs = getAliveNpcs();

        for (Npc* npc : alive_npcs) {
            if (!npc->isAlive()) continue;

            int moveDistance = npc->getMoveDistance();
            
            int dx = dir_dist(gen) * (std::uniform_int_distribution<>(0, moveDistance)(gen));
            int dy = dir_dist(gen) * (std::uniform_int_distribution<>(0, moveDistance)(gen));

            int newX = npc->getX() + dx;
            int newY = npc->getY() + dy;

            if (isValidPosition(newX, newY)) {
                npc->setPosition(newX, newY);
            }
        }

        alive_npcs = getAliveNpcs();
        CombatVisitor visitor;

        for (size_t i = 0; i < alive_npcs.size(); ++i) {
            for (size_t j = i + 1; j < alive_npcs.size(); ++j) {
                Npc* npc1 = alive_npcs[i];
                Npc* npc2 = alive_npcs[j];

                if (!npc1->isAlive() || !npc2->isAlive()) continue;

                double distance = npc1->distanceTo(*npc2);
                int killDist = std::max(npc1->getKillDistance(), npc2->getKillDistance());

                if (distance <= killDist) {
                    if (visitor.canKill(npc1, npc2) || visitor.canKill(npc2, npc1)) {
                        {
                            std::lock_guard<std::mutex> lock(battle_queue_mutex_);
                            battle_queue_.push({npc1, npc2});
                        }
                        battle_cv_.notify_one();
                    }
                }
            }
        }
    }
}

void Arena::battleThreadFunc() {
    while (running_) {
        std::unique_lock<std::mutex> lock(battle_queue_mutex_);
        battle_cv_.wait_for(lock, std::chrono::milliseconds(100), [this] { 
            return !battle_queue_.empty() || !running_; 
        });

        if (!battle_queue_.empty()) {
            BattleTask task = battle_queue_.front();
            battle_queue_.pop();
            lock.unlock();

            Npc* attacker = task.attacker;
            Npc* defender = task.defender;

            if (!attacker->isAlive() || !defender->isAlive()) continue;

            CombatVisitor visitor;
            bool attackerCanKill = visitor.canKill(attacker, defender);
            bool defenderCanKill = visitor.canKill(defender, attacker);

            if (attackerCanKill && defenderCanKill) {
                int attackPower1 = rollDice();
                int defensePower1 = rollDice();
                int attackPower2 = rollDice();
                int defensePower2 = rollDice();

                bool attacker_wins = attackPower1 > defensePower2;
                bool defender_wins = attackPower2 > defensePower1;

                if (attacker_wins && defender_wins) {
                    attacker->kill();
                    defender->kill();
                    std::stringstream ss;
                    ss << attacker->getName() << " (" << attacker->getType() 
                       << ") and " << defender->getName() << " (" << defender->getType() 
                       << ") killed each other [" << attackPower1 << " vs " << defensePower2 
                       << ", " << attackPower2 << " vs " << defensePower1 << "]";
                    notifyObservers(ss.str());
                } else if (attacker_wins) {
                    defender->kill();
                    std::stringstream ss;
                    ss << attacker->getName() << " (" << attacker->getType() 
                       << ") killed " << defender->getName() << " (" << defender->getType() 
                       << ") [" << attackPower1 << " > " << defensePower2 << "]";
                    notifyObservers(ss.str());
                } else if (defender_wins) {
                    attacker->kill();
                    std::stringstream ss;
                    ss << defender->getName() << " (" << defender->getType() 
                       << ") killed " << attacker->getName() << " (" << attacker->getType() 
                       << ") [" << attackPower2 << " > " << defensePower1 << "]";
                    notifyObservers(ss.str());
                }
            } else if (attackerCanKill) {
                int attackPower = rollDice();
                int defensePower = rollDice();

                if (attackPower > defensePower) {
                    defender->kill();
                    std::stringstream ss;
                    ss << attacker->getName() << " (" << attacker->getType() 
                       << ") killed " << defender->getName() << " (" << defender->getType() 
                       << ") [" << attackPower << " > " << defensePower << "]";
                    notifyObservers(ss.str());
                }
            } else if (defenderCanKill) {
                int attackPower = rollDice();
                int defensePower = rollDice();

                if (attackPower > defensePower) {
                    attacker->kill();
                    std::stringstream ss;
                    ss << defender->getName() << " (" << defender->getType() 
                       << ") killed " << attacker->getName() << " (" << attacker->getType() 
                       << ") [" << attackPower << " > " << defensePower << "]";
                    notifyObservers(ss.str());
                }
            }
        }
    }
}

void Arena::printThreadFunc(int durationSeconds) {
    for (int i = 0; i < durationSeconds && running_; ++i) {
        printMap();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Arena::startGame(int durationSeconds) {
    if (running_) {
        throw std::runtime_error("Game is already running");
    }

    running_ = true;
    movement_thread_ = std::thread(&Arena::movementThreadFunc, this);
    battle_thread_ = std::thread(&Arena::battleThreadFunc, this);
    print_thread_ = std::thread(&Arena::printThreadFunc, this, durationSeconds);
    print_thread_.join();
    stopGame();
}

void Arena::stopGame() {
    if (!running_) return;

    running_ = false;
    battle_cv_.notify_all();

    if (movement_thread_.joinable()) movement_thread_.join();
    if (battle_thread_.joinable()) battle_thread_.join();
}