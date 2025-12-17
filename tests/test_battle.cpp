#include <gtest/gtest.h>
#include "../include/arena.h"
#include "../include/factory.h"
#include "../include/combat_visitor.h"
#include "../include/dragon.h"
#include "../include/elf.h"
#include "../include/druid.h"

TEST(AsyncBattleTest, DragonKillsElf) {
    CombatVisitor visitor;
    Dragon dragon(0, 0, "Dragon");
    Elf elf(5, 5, "Elf");
    
    EXPECT_TRUE(visitor.canKill(&dragon, &elf));
}

TEST(AsyncBattleTest, DragonDoesNotKillDruid) {
    CombatVisitor visitor;
    Dragon dragon(0, 0, "Dragon");
    Druid druid(5, 5, "Druid");
    
    EXPECT_FALSE(visitor.canKill(&dragon, &druid));
}

TEST(AsyncBattleTest, DragonDoesNotKillDragon) {
    CombatVisitor visitor;
    Dragon dragon1(0, 0, "Dragon1");
    Dragon dragon2(5, 5, "Dragon2");
    
    EXPECT_FALSE(visitor.canKill(&dragon1, &dragon2));
}

TEST(AsyncBattleTest, ElfKillsDruid) {
    CombatVisitor visitor;
    Elf elf(0, 0, "Elf");
    Druid druid(5, 5, "Druid");
    
    EXPECT_TRUE(visitor.canKill(&elf, &druid));
}

TEST(AsyncBattleTest, ElfDoesNotKillDragon) {
    CombatVisitor visitor;
    Elf elf(0, 0, "Elf");
    Dragon dragon(5, 5, "Dragon");
    
    EXPECT_FALSE(visitor.canKill(&elf, &dragon));
}

TEST(AsyncBattleTest, ElfDoesNotKillElf) {
    CombatVisitor visitor;
    Elf elf1(0, 0, "Elf1");
    Elf elf2(5, 5, "Elf2");
    
    EXPECT_FALSE(visitor.canKill(&elf1, &elf2));
}

TEST(AsyncBattleTest, DruidKillsDragon) {
    CombatVisitor visitor;
    Druid druid(0, 0, "Druid");
    Dragon dragon(5, 5, "Dragon");
    
    EXPECT_TRUE(visitor.canKill(&druid, &dragon));
}

TEST(AsyncBattleTest, DruidDoesNotKillElf) {
    CombatVisitor visitor;
    Druid druid(0, 0, "Druid");
    Elf elf(5, 5, "Elf");
    
    EXPECT_FALSE(visitor.canKill(&druid, &elf));
}

TEST(AsyncBattleTest, DruidDoesNotKillDruid) {
    CombatVisitor visitor;
    Druid druid1(0, 0, "Druid1");
    Druid druid2(5, 5, "Druid2");
    
    EXPECT_FALSE(visitor.canKill(&druid1, &druid2));
}

TEST(AsyncBattleTest, NpcCannotKillItself) {
    CombatVisitor visitor;
    Dragon dragon(0, 0, "Dragon");
    Elf elf(0, 0, "Elf");
    Druid druid(0, 0, "Druid");
    
    EXPECT_FALSE(visitor.canKill(&dragon, &dragon));
    EXPECT_FALSE(visitor.canKill(&elf, &elf));
    EXPECT_FALSE(visitor.canKill(&druid, &druid));
}

TEST(AsyncBattleTest, DragonKillDistance) {
    Dragon dragon(0, 0, "Dragon");
    EXPECT_EQ(dragon.getKillDistance(), 30);
}

TEST(AsyncBattleTest, ElfKillDistance) {
    Elf elf(0, 0, "Elf");
    EXPECT_EQ(elf.getKillDistance(), 50);
}

TEST(AsyncBattleTest, DruidKillDistance) {
    Druid druid(0, 0, "Druid");
    EXPECT_EQ(druid.getKillDistance(), 10);
}

TEST(AsyncBattleTest, DragonMoveDistance) {
    Dragon dragon(0, 0, "Dragon");
    EXPECT_EQ(dragon.getMoveDistance(), 50);
}

TEST(AsyncBattleTest, ElfMoveDistance) {
    Elf elf(0, 0, "Elf");
    EXPECT_EQ(elf.getMoveDistance(), 10);
}

TEST(AsyncBattleTest, DruidMoveDistance) {
    Druid druid(0, 0, "Druid");
    EXPECT_EQ(druid.getMoveDistance(), 10);
}

TEST(AsyncBattleTest, BattleRulesComplete) {
    CombatVisitor visitor;
    
    Dragon dragon(0, 0, "Dragon");
    Elf elf(5, 5, "Elf");
    Druid druid(10, 10, "Druid");
    
    EXPECT_TRUE(visitor.canKill(&dragon, &elf));
    EXPECT_FALSE(visitor.canKill(&dragon, &druid));
    
    EXPECT_TRUE(visitor.canKill(&elf, &druid));
    EXPECT_FALSE(visitor.canKill(&elf, &dragon));
    
    EXPECT_TRUE(visitor.canKill(&druid, &dragon));
    EXPECT_FALSE(visitor.canKill(&druid, &elf));
}

TEST(AsyncBattleTest, ArenaBattleShortGame) {
    Arena arena(100, 100);
    
    arena.createAndAddNpc("Dragon", "Dragon1", 50, 50);
    arena.createAndAddNpc("Elf", "Elf1", 55, 50);
    
    EXPECT_EQ(arena.getAliveCount(), 2);
    
    arena.startGame(2);
    
    EXPECT_LE(arena.getAliveCount(), 2);
}

TEST(AsyncBattleTest, ArenaBattleDruidVsDragon) {
    Arena arena(100, 100);
    
    arena.createAndAddNpc("Druid", "Druid1", 50, 50);
    arena.createAndAddNpc("Dragon", "Dragon1", 55, 50);
    
    EXPECT_EQ(arena.getAliveCount(), 2);
    
    arena.startGame(2);
    
    EXPECT_LE(arena.getAliveCount(), 1);
}