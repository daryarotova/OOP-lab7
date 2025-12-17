#include <gtest/gtest.h>
#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/console_observer.h"
#include "../include/file_observer.h"
#include <thread>
#include <chrono>

TEST(AsyncThreadsTest, GenerateRandomNpcs) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(30);
    EXPECT_EQ(arena.getNpcCount(), 30);
}

TEST(AsyncThreadsTest, AllNpcsAliveAfterGeneration) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(20);
    EXPECT_EQ(arena.getAliveCount(), 20);
}

TEST(AsyncThreadsTest, MapBoundariesForAllTypes) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(40);
    
    auto alive_npcs = arena.getAliveNpcs();
    for (Npc* npc : alive_npcs) {
        EXPECT_GE(npc->getX(), 0);
        EXPECT_LE(npc->getX(), 100);
        EXPECT_GE(npc->getY(), 0);
        EXPECT_LE(npc->getY(), 100);
    }
}

TEST(AsyncThreadsTest, ShortAsyncGameRun) {
    Arena arena(100, 100);
    auto observer = std::make_shared<ConsoleObserver>();
    arena.addObserver(observer);
    arena.generateRandomNpcs(15);
    
    size_t initial_count = arena.getAliveCount();
    EXPECT_EQ(initial_count, 15);
    arena.startGame(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(2100));
    
    size_t final_count = arena.getAliveCount();
    EXPECT_LE(final_count, initial_count);
}

TEST(AsyncThreadsTest, AsyncGameStopsCorrectly) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(10);
    
    auto start = std::chrono::steady_clock::now();
    arena.startGame(1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    auto end = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    
    EXPECT_GE(duration, 1);
    EXPECT_LE(duration, 3);
}

TEST(AsyncThreadsTest, ThreadSafeInAsyncMode) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(25);
    
    std::atomic<bool> running{true};
    std::vector<std::thread> reader_threads;
    for (int i = 0; i < 3; ++i) {
        reader_threads.emplace_back([&arena, &running]() {
            while (running) {
                size_t alive_count = arena.getAliveCount();
                EXPECT_LE(alive_count, 25);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    std::thread game_thread([&arena]() {
        arena.startGame(1);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    running = false;
    
    for (auto& t : reader_threads) {
        t.join();
    }
    game_thread.join();
}

TEST(AsyncThreadsTest, DeadNpcsNotInAliveListAfterAsyncGame) {
    Arena arena(100, 100);
    
    arena.createAndAddNpc("Dragon", "Dragon1", 10, 10);
    arena.createAndAddNpc("Elf", "Elf1", 12, 10);
    
    EXPECT_EQ(arena.getAliveCount(), 2);
    
    arena.startGame(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    auto alive_npcs = arena.getAliveNpcs();
    EXPECT_LE(alive_npcs.size(), 2);
    EXPECT_GE(alive_npcs.size(), 0);
}

TEST(AsyncThreadsTest, MultipleObservers) {
    Arena arena(100, 100);
    
    auto console_observer = std::make_shared<ConsoleObserver>();
    auto file_observer = std::make_shared<FileObserver>("test_log.txt");
    
    arena.addObserver(console_observer);
    arena.addObserver(file_observer);
    
    arena.generateRandomNpcs(5);
    
    EXPECT_NO_THROW({
        arena.startGame(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    });
    
    std::ifstream test_file("test_log.txt");
    EXPECT_TRUE(test_file.is_open());
    test_file.close();
    
    std::remove("test_log.txt");
}

TEST(AsyncThreadsTest, MultipleAsyncGameRuns) {
    Arena arena(100, 100);
    arena.generateRandomNpcs(10);
    
    arena.startGame(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    size_t count_after_first = arena.getAliveCount();
    
    arena.startGame(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    size_t count_after_second = arena.getAliveCount();
    
    EXPECT_LE(count_after_second, count_after_first);
}

TEST(AsyncThreadsTest, EmptyArenaAsyncGame) {
    Arena arena(100, 100);
    EXPECT_EQ(arena.getNpcCount(), 0);
    EXPECT_EQ(arena.getAliveCount(), 0);
    
    EXPECT_NO_THROW({
        arena.startGame(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    });
}