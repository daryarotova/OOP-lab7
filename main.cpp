#include "include/arena.h"
#include "include/factory.h"
#include "include/console_observer.h"
#include "include/file_observer.h"
#include <iostream>
#include <memory>

int main() {
    try {
        std::cout << "=== Asynchronous NPC Battle ===" << std::endl;
        std::cout << std::endl;

        Arena arena(100, 100);

        auto consoleObserver = std::make_shared<ConsoleObserver>();
        auto fileObserver = std::make_shared<FileObserver>("battle_log.txt");
        arena.addObserver(consoleObserver);
        arena.addObserver(fileObserver);

        std::cout << "Generating 50 random NPCs on 100x100 map..." << std::endl;
        arena.generateRandomNpcs(50);
        std::cout << "Created NPCs: " << arena.getNpcCount() << std::endl;
        std::cout << std::endl;

        std::cout << "NPC Parameters:" << std::endl;
        std::cout << "  Dragon: Move=50, Kill=30" << std::endl;
        std::cout << "  Elf:    Move=10, Kill=50" << std::endl;
        std::cout << "  Druid:  Move=10, Kill=10" << std::endl;
        std::cout << std::endl;

        std::cout << "Starting game for 30 seconds..." << std::endl;
        std::cout << "Threads:" << std::endl;
        std::cout << "  1. NPC movement thread (collision detection)" << std::endl;
        std::cout << "  2. Battle system thread (dice rolls)" << std::endl;
        std::cout << "  3. Map output thread (every second)" << std::endl;
        std::cout << std::endl;
        std::cout << "Map legend: D=Dragon, E=Elf, R=Druid, .=empty" << std::endl;
        std::cout << "==========================================================\n" << std::endl;

        arena.startGame(30);

        std::cout << "\nChecking if battle log file exists..." << std::endl;
        std::ifstream test_file("battle_log.txt");
        if (test_file.is_open()) {
            std::cout << "Battle log file successfully created!" << std::endl;
            test_file.close();
        } else {
            std::cout << "WARNING: Battle log file was not created!" << std::endl;
        }

        std::cout << "\n==========================================================\n";
        std::cout << "Game finished!" << std::endl;
        std::cout << std::endl;
        arena.printSurvivors();

        std::cout << "Logs saved to file 'battle_log.txt'" << std::endl;
        std::cout << "=== Program completed successfully ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}